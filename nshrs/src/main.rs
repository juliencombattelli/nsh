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
        fn get_entry(&self, age: usize) -> Option<&str>;
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
        use nostd::collections::FixedString;

        pub struct HistoryRing<const CAP: usize, const STRING_SIZE: usize> {
            head: usize,
            tail: usize,
            size: usize,
            buffer: [FixedString<STRING_SIZE>; CAP],
        }
        impl<const CAP: usize, const STRING_SIZE: usize> HistoryRing<CAP, STRING_SIZE> {
            pub fn reset(&mut self) {
                self.head = 0;
                self.tail = 0;
                self.size = 0;
                // No need to reset the string buffer
                // self.buffer = [FixedString::from(""); CAP];
            }
            pub fn entry_count(&self) -> usize {
                self.size
            }
            pub fn is_full(&self) -> bool {
                self.size == CAP
            }
            pub fn is_empty(&self) -> bool {
                self.size == 0
            }
            pub fn push_entry(&mut self, entry: &str) {
                self.buffer[self.head] = FixedString::from(entry);
                self.head = (self.head + 1) % CAP;
                if self.is_full() {
                    self.tail = (self.tail + 1) % CAP;
                } else {
                    self.size += 1;
                }
            }
            pub fn get_entry(&self, age: usize) -> Option<&str> {
                if age >= self.entry_count() {
                    None
                } else {
                    let most_recent_entry = self.head - 1; // head points to the element just after the most recent entry
                    let mut entry_index = most_recent_entry - age;
                    if entry_index > CAP {
                        entry_index += CAP; // deal with ring buffer rollover
                                            // TODO is there any wrapping_add in Rust for unsigned integers?
                    }
                    // TODO indexing may be in an unsafe block even if it is safe
                    Some(self.buffer[entry_index].as_ref())
                }
            }
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
