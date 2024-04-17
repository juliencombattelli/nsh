use core::mem::MaybeUninit;
use core::option::Option;
use std::mem;

pub struct FixedRingBuffer<T, const CAP: usize> {
    buffer: [MaybeUninit<T>; CAP],
    read_index: usize,
    write_index: usize,
}

impl<T, const CAP: usize> FixedRingBuffer<T, CAP> {
    const ERROR_CAPACITY_SHALL_NOT_BE_ZERO: () = assert!(CAP != 0);

    pub fn new() -> Self {
        Self {
            buffer: unsafe { MaybeUninit::uninit().assume_init() },
            read_index: 0,
            write_index: 0,
        }
    }

    pub fn push(&mut self, value: T) {
        if self.is_full() {
            let index = self.read_index % CAP;
            let previous_value = mem::replace(&mut self.buffer[index], MaybeUninit::uninit());
            unsafe {
                drop(previous_value.assume_init());
            }
            self.read_index += 1;
        }
        let index = self.write_index % CAP;
        self.buffer[index] = MaybeUninit::new(value);
        self.write_index += 1;
    }

    pub fn pop(&mut self) -> Option<T> {
        if self.is_empty() {
            None
        } else {
            let index = self.read_index % CAP;
            let element = mem::replace(&mut self.buffer[index], MaybeUninit::uninit());
            self.read_index += 1;
            unsafe { Some(element.assume_init()) }
        }
    }

    pub const fn capacity(&self) -> usize {
        CAP
    }

    pub fn len(&self) -> usize {
        self.write_index - self.read_index
    }

    pub fn is_full(&self) -> bool {
        self.len() == self.capacity()
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn new() {
        let rb = FixedRingBuffer::<i32, 16>::new();
        assert_eq!(rb.capacity(), 16);
        assert_eq!(rb.len(), 0);
        assert_eq!(rb.is_empty(), true);
        assert_eq!(rb.is_full(), false);
    }

    #[test]
    fn pop_empty() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        let i = rb.pop();
        assert_eq!(i, None);
    }

    #[test]
    fn push_single() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        rb.push(42);
        assert_eq!(rb.len(), 1);
        assert_eq!(rb.is_empty(), false);
        assert_eq!(rb.is_full(), false);
    }

    #[test]
    fn push_n() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=10 {
            rb.push(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
            assert_eq!(rb.is_full(), false);
        }
    }

    #[test]
    fn push_n_full() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=16 {
            rb.push(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
            if i == 16 {
                assert_eq!(rb.is_full(), true);
            } else {
                assert_eq!(rb.is_full(), false);
            }
        }
    }

    #[test]
    fn push_n_full_wrap_around() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=(rb.capacity() as i32) {
            rb.push(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
            if i == 16 {
                assert_eq!(rb.is_full(), true);
            } else {
                assert_eq!(rb.is_full(), false);
            }
        }
        for i in 17..=22 {
            rb.push(i);
            assert_eq!(rb.len(), rb.capacity());
            assert_eq!(rb.is_empty(), false);
            assert_eq!(rb.is_full(), true);
        }
    }
}
