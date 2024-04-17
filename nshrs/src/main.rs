mod fixed_string;

use ringbuffer::GrowableAllocRingBuffer as FixedRingBuffer;

pub mod nsh {
    #[macro_export]
    macro_rules! nsh_static_dispatcher {
        ($(($cmd_name:expr, $cmd_fn:expr)),*) => {
            |cmd, args| {
                match cmd {
                    $($cmd_name => { $cmd_fn(args) },)+
                    _ => unreachable!(),
                }
            }
        }
    }

    pub type CmdFn = fn(args: &str);

    pub trait Dispatcher {
        fn dispatch(&self, cmd: &str, args: &str);
    }

    pub trait History {
        fn reset(&mut self);
        fn entry_count(&self) -> usize;
        fn is_full(&self) -> bool;
        fn is_empty(&self) -> bool;
        fn push_entry(&mut self, entry: &str);
    }

    pub struct Shell<D: Dispatcher> {
        dispatcher: D,
    }
    impl<D: Dispatcher> Shell<D> {
        pub fn new(dispatcher: D) -> Self {
            Self { dispatcher }
        }
    }

    pub mod static_dispatcher {

        use crate::nsh::CmdFn;
        use crate::nsh::Dispatcher;

        pub type StaticDispatcherCallbackFn = fn(cmd: &str, args: &str);
        pub struct StaticDispatcherCallback {
            callback: StaticDispatcherCallbackFn,
        }
        impl StaticDispatcherCallback {
            pub fn new(callback: StaticDispatcherCallbackFn) -> Self {
                Self { callback }
            }
        }
        impl Dispatcher for StaticDispatcherCallback {
            fn dispatch(&self, cmd: &str, args: &str) {
                (self.callback)(cmd, args);
            }
        }

        pub type CmdArrayRef = &'static [(&'static str, CmdFn)];
        pub struct StaticDispatcherArrayRef {
            commands: CmdArrayRef,
        }
        impl StaticDispatcherArrayRef {
            pub fn new(commands: CmdArrayRef) -> Self {
                Self { commands }
            }
        }
        impl Dispatcher for StaticDispatcherArrayRef {
            fn dispatch(&self, cmd: &str, args: &str) {
                for (cmd_name, cmd_fn) in self.commands {
                    if *cmd_name == cmd {
                        cmd_fn(args);
                    }
                }
            }
        }

        pub type CmdArray<const CAP: usize> = [(&'static str, CmdFn); CAP];
        pub struct StaticDispatcherArray<const CAP: usize> {
            commands: CmdArray<CAP>,
        }
        impl<const CAP: usize> StaticDispatcherArray<CAP> {
            pub fn new(commands: CmdArray<CAP>) -> Self {
                Self { commands }
            }
        }
        impl<const CAP: usize> Dispatcher for StaticDispatcherArray<CAP> {
            fn dispatch(&self, cmd: &str, args: &str) {
                for (cmd_name, cmd_fn) in self.commands {
                    if cmd_name == cmd {
                        cmd_fn(args);
                    }
                }
            }
        }
    }

    pub mod history {
        use crate::fixed_string::FixedString;

        pub struct HistoryRing<const CAP: usize, const STRING_SIZE: usize> {
            head: usize,
            len: usize,
            buffer: [FixedString<STRING_SIZE>; CAP],
        }
    }
}

pub fn main() {
    use nsh::static_dispatcher::StaticDispatcherArray;
    use nsh::static_dispatcher::StaticDispatcherArrayRef;
    use nsh::static_dispatcher::StaticDispatcherCallback;
    use nsh::Shell;

    const COMMANDS: nsh::static_dispatcher::CmdArrayRef = &[
        ("help", |_: &str| {
            println!("help");
        }),
        ("cd", |_: &str| {
            println!("cd");
        }),
        ("ls", |_: &str| {
            println!("ls");
        }),
    ];

    const COMMANDS_SIZED: nsh::static_dispatcher::CmdArray<3> = [
        ("help", |_: &str| {
            println!("help");
        }),
        ("cd", |_: &str| {
            println!("cd");
        }),
        ("ls", |_: &str| {
            println!("ls");
        }),
    ];

    // Example with a dispatcher that uses a callback to dispatch commands
    let _ = Shell::new(StaticDispatcherCallback::new(nsh_static_dispatcher![
        ("help", |_: &str| {
            println!("help");
        }),
        ("cd", |_: &str| {
            println!("cd");
        }),
        ("ls", |_: &str| {
            println!("ls");
        })
    ]));

    // Example with a dispatcher referencing a constant command array
    let _ = Shell::new(StaticDispatcherArrayRef::new(COMMANDS));

    // Example with a dispatcher referencing a literal command array
    let _ = Shell::new(StaticDispatcherArrayRef::new(&[
        ("help", |_: &str| {
            println!("help");
        }),
        ("cd", |_: &str| {
            println!("cd");
        }),
        ("ls", |_: &str| {
            println!("ls");
        }),
    ]));

    // Example with a dispatcher owning the command array, copied from constant
    let _ = Shell::new(StaticDispatcherArray::new(COMMANDS_SIZED));

    // Example with a dispatcher owning the command array, copied from literal
    let _ = Shell::new(StaticDispatcherArray::new([
        ("help", |_: &str| {
            println!("help");
        }),
        ("cd", |_: &str| {
            println!("cd");
        }),
        ("ls", |_: &str| {
            println!("ls");
        }),
    ]));
}
