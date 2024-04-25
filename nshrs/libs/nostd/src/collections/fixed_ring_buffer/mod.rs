//! A queue implemented with a fixed-capacity ring buffer allocated on the stack.
//!
//! Based on https://github.com/NULLx76/ringbuffer

use core::ops::{Index, IndexMut};

pub trait RingBuffer<T>:
    Sized + IntoIterator<Item = T> + Extend<T> + Index<usize, Output = T> + IndexMut<usize>
{
    fn len(&self) -> usize;

    fn capacity(&self) -> usize;

    fn is_empty(&self) -> bool {
        self.len() == 0
    }

    fn is_full(&self) -> bool {
        self.len() == self.capacity()
    }

    fn get(&self, index: usize) -> Option<&T>;

    fn get_mut(&mut self, index: usize) -> Option<&mut T>;

    fn front(&self) -> Option<&T>;

    fn front_mut(&mut self) -> Option<&mut T>;

    fn back(&self) -> Option<&T>;

    fn back_mut(&mut self) -> Option<&mut T>;

    fn contains(&self, elem: &T) -> bool
    where
        T: PartialEq,
    {
        self.iter().any(|i| i == elem)
    }

    fn push_back(&mut self, value: T);

    fn pop_front(&mut self) -> Option<T>;

    fn clear(&mut self);

    fn fill(&mut self, value: T)
    where
        T: Clone,
    {
        self.fill_with(|| value.clone());
    }

    fn fill_with<F: FnMut() -> T>(&mut self, f: F);

    fn fill_with_default(&mut self)
    where
        T: Default,
    {
        self.fill_with(Default::default);
    }

    fn iter(&self) -> Iter<T> {
        let (start, end) = self.as_slices();
        Iter::new(start.iter(), end.iter())
    }

    // fn iter_mut(&mut self) -> RingBufferMutIterator<T, Self> {
    //     RingBufferMutIterator::new(self)
    // }

    fn skip(&mut self) {
        let _ = self.pop_front();
    }

    fn drain(&mut self) -> RingBufferDrainingIterator<T, Self> {
        RingBufferDrainingIterator::new(self)
    }

    fn as_slices(&self) -> (&[T], &[T]);

    fn as_mut_slices(&mut self) -> (&mut [T], &mut [T]) {
        (&mut [], &mut [])
    }

    fn make_contiguous(&mut self) -> &mut [T] {
        &mut []
    }
}

mod iter {
    use super::RingBuffer;
    use core::iter::FusedIterator;
    use core::marker::PhantomData;
    use core::mem;
    use core::slice;

    pub struct Iter<'rb, T: 'rb> {
        start: slice::Iter<'rb, T>,
        end: slice::Iter<'rb, T>,
    }

    impl<'rb, T> Iter<'rb, T> {
        pub fn new(start: slice::Iter<'rb, T>, end: slice::Iter<'rb, T>) -> Self {
            Self { start, end }
        }
    }

    impl<'rb, T: 'rb> Iterator for Iter<'rb, T> {
        type Item = &'rb T;

        fn next(&mut self) -> Option<Self::Item> {
            match self.start.next() {
                Some(val) => Some(val),
                None => {
                    mem::swap(&mut self.start, &mut self.end);
                    self.start.next()
                }
            }
        }
    }

    impl<'rb, T: 'rb> FusedIterator for Iter<'rb, T> {}

    impl<'rb, T: 'rb> ExactSizeIterator for Iter<'rb, T> {}

    // pub struct RingBufferMutIterator<'rb, T, RB: RingBuffer<T>> {
    //     ring_buffer: &'rb mut RB,
    //     index: usize,
    //     data_start: &'rb mut [T],
    //     data_end: &'rb mut [T],
    //     phantom: PhantomData<&'rb mut T>,
    // }

    // impl<'rb, T, RB: RingBuffer<T>> RingBufferMutIterator<'rb, T, RB> {
    //     pub fn new(ring_buffer: &'rb mut RB) -> Self {
    //         Self {
    //             ring_buffer,
    //             index: 0,
    //             phantom: PhantomData,
    //         }
    //     }
    // }

    // impl<'rb, T: 'rb, RB: RingBuffer<T> + 'rb> FusedIterator for RingBufferMutIterator<'rb, T, RB> {}

    // impl<'rb, T: 'rb, RB: RingBuffer<T> + 'rb> ExactSizeIterator for RingBufferMutIterator<'rb, T, RB> {}

    // impl<'rb, T, RB: RingBuffer<T> + 'rb> Iterator for RingBufferMutIterator<'rb, T, RB> {
    //     type Item = &'rb mut T;

    //     fn next(&mut self) -> Option<Self::Item> {
    //         if self.index < self.ring_buffer.len() {
    //             let res = self.ring_buffer.get_mut(self.index);
    //             self.index += 1;
    //             res
    //         } else {
    //             None
    //         }
    //     }
    // }

    pub struct RingBufferDrainingIterator<'rb, T, RB: RingBuffer<T>> {
        ring_buffer: &'rb mut RB,
        phantom: PhantomData<T>,
    }

    impl<'rb, T, RB: RingBuffer<T>> RingBufferDrainingIterator<'rb, T, RB> {
        pub fn new(ring_buffer: &'rb mut RB) -> Self {
            Self {
                ring_buffer,
                phantom: PhantomData,
            }
        }
    }

    impl<'rb, T, RB: RingBuffer<T>> Iterator for RingBufferDrainingIterator<'rb, T, RB> {
        type Item = T;

        fn next(&mut self) -> Option<T> {
            self.ring_buffer.pop_front()
        }
    }

    pub struct RingBufferIntoIterator<T, RB: RingBuffer<T>> {
        ring_buffer: RB,
        phantom: PhantomData<T>,
    }

    impl<T, RB: RingBuffer<T>> RingBufferIntoIterator<T, RB> {
        pub fn new(ring_buffer: RB) -> Self {
            Self {
                ring_buffer,
                phantom: PhantomData,
            }
        }
    }

    impl<T, RB: RingBuffer<T>> Iterator for RingBufferIntoIterator<T, RB> {
        type Item = T;

        fn next(&mut self) -> Option<Self::Item> {
            self.ring_buffer.pop_front()
        }
    }
}

pub use iter::{
    Iter,
    //RingBufferMutIterator,
    RingBufferDrainingIterator,
    RingBufferIntoIterator,
};

use core::iter::FromIterator;
use core::mem;
use core::mem::MaybeUninit;

#[derive(Debug)]
pub struct FixedRingBuffer<T, const CAP: usize> {
    buffer: [MaybeUninit<T>; CAP],
    read_index: usize,
    write_index: usize,
}

impl<T, const CAP: usize> From<[T; CAP]> for FixedRingBuffer<T, CAP> {
    fn from(value: [T; CAP]) -> Self {
        Self {
            // Safety:
            // TODO Ensure this is actually safe
            buffer: unsafe { mem::transmute_copy(&value) },
            read_index: 0,
            write_index: CAP,
        }
    }
}

impl<T: Clone, const CAP: usize> From<&[T; CAP]> for FixedRingBuffer<T, CAP> {
    fn from(value: &[T; CAP]) -> Self {
        Self::from(value.clone())
    }
}

impl<T: Clone, const CAP: usize> From<&[T]> for FixedRingBuffer<T, CAP> {
    fn from(value: &[T]) -> Self {
        value.iter().cloned().collect()
    }
}

impl<T: Clone, const CAP: usize> From<&mut [T; CAP]> for FixedRingBuffer<T, CAP> {
    fn from(value: &mut [T; CAP]) -> Self {
        Self::from(value.clone())
    }
}

impl<T: Clone, const CAP: usize> From<&mut [T]> for FixedRingBuffer<T, CAP> {
    fn from(value: &mut [T]) -> Self {
        value.iter().cloned().collect()
    }
}

impl<const CAP: usize> From<&str> for FixedRingBuffer<char, CAP> {
    fn from(value: &str) -> Self {
        value.chars().collect()
    }
}

impl<T, const CAP: usize> Drop for FixedRingBuffer<T, CAP> {
    fn drop(&mut self) {
        self.drain().for_each(drop);
    }
}

impl<T: Clone, const CAP: usize> Clone for FixedRingBuffer<T, CAP> {
    fn clone(&self) -> Self {
        let mut new = FixedRingBuffer::<T, CAP>::new();
        self.iter().cloned().for_each(|i| new.push_back(i));
        new
    }
}

impl<T: PartialEq, const CAP: usize> PartialEq for FixedRingBuffer<T, CAP> {
    fn eq(&self, other: &Self) -> bool {
        if self.len() == other.len() {
            for (a, b) in self.iter().zip(other.iter()) {
                if a != b {
                    return false;
                }
            }
            true
        } else {
            false
        }
    }
}

impl<T: PartialEq, const CAP: usize> Eq for FixedRingBuffer<T, CAP> {}

impl<T, const CAP: usize> FixedRingBuffer<T, CAP> {
    const ERROR_CAPACITY_MUST_NOT_BE_ZERO: () = assert!(CAP != 0, "Capacity must not be zero");

    pub fn new() -> Self {
        let _ = Self::ERROR_CAPACITY_MUST_NOT_BE_ZERO;

        Self {
            // TODO Replace with uninit_array once the feature is stabilized
            buffer: unsafe { MaybeUninit::<[MaybeUninit<T>; CAP]>::uninit().assume_init() },
            read_index: 0,
            write_index: 0,
        }
    }
}

// TODO Replace with MaybeUninit<T>::slice_assume_init_ref once the feature is stabilized
const unsafe fn maybe_uninit_slice_assume_init_ref<T>(slice: &[MaybeUninit<T>]) -> &[T] {
    unsafe { &*(slice as *const [MaybeUninit<T>] as *const [T]) }
}

impl<T, const CAP: usize> IntoIterator for FixedRingBuffer<T, CAP> {
    type Item = T;
    type IntoIter = RingBufferIntoIterator<T, Self>;

    fn into_iter(self) -> Self::IntoIter {
        RingBufferIntoIterator::new(self)
    }
}

impl<'a, T, const CAP: usize> IntoIterator for &'a FixedRingBuffer<T, CAP> {
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

// impl<'a, T, const CAP: usize> IntoIterator for &'a mut FixedRingBuffer<T, CAP> {
//     type Item = &'a mut T;
//     type IntoIter = RingBufferMutIterator<'a, T>;

//     fn into_iter(self) -> Self::IntoIter {
//         self.iter_mut()
//     }
// }

impl<T, const CAP: usize> Extend<T> for FixedRingBuffer<T, CAP> {
    fn extend<A: IntoIterator<Item = T>>(&mut self, iter: A) {
        let iter = iter.into_iter();

        for i in iter {
            self.push_back(i);
        }
    }
}

impl<T, const CAP: usize> RingBuffer<T> for FixedRingBuffer<T, CAP> {
    fn len(&self) -> usize {
        self.write_index - self.read_index
    }

    fn capacity(&self) -> usize {
        CAP
    }

    fn get(&self, index: usize) -> Option<&T> {
        if index < self.len() {
            // Safety: elements in buffer are always initialized when inserted
            // and the length of the ring buffer is updated accordingly, so if
            // we index the buffer in its valid range then the referenced
            // element is guaranteed to be initialized
            Some(unsafe { self.buffer[index].assume_init_ref() })
        } else {
            None
        }
    }

    fn get_mut(&mut self, index: usize) -> Option<&mut T> {
        if index < self.len() {
            // Safety: elements in buffer are always initialized when inserted
            // and the length of the ring buffer is updated accordingly, so if
            // we index the buffer in its valid range then the referenced
            // element is guaranteed to be initialized
            Some(unsafe { self.buffer[index].assume_init_mut() })
        } else {
            None
        }
    }

    fn front(&self) -> Option<&T> {
        if self.is_empty() {
            None
        } else {
            Some(unsafe { self.buffer[self.read_index].assume_init_ref() })
        }
    }

    fn front_mut(&mut self) -> Option<&mut T> {
        if self.is_empty() {
            None
        } else {
            Some(unsafe { self.buffer[self.read_index].assume_init_mut() })
        }
    }

    fn back(&self) -> Option<&T> {
        if self.is_empty() {
            None
        } else {
            Some(unsafe { self.buffer[self.write_index].assume_init_ref() })
        }
    }

    fn back_mut(&mut self) -> Option<&mut T> {
        if self.is_empty() {
            None
        } else {
            Some(unsafe { self.buffer[self.write_index].assume_init_mut() })
        }
    }

    fn push_back(&mut self, value: T) {
        if self.is_full() {
            let previous_value = mem::replace(
                &mut self.buffer[self.read_index % CAP],
                MaybeUninit::uninit(),
            );
            // make sure we drop whatever is being overwritten
            // Safety: the buffer is full, so previous_value is already initialized
            unsafe {
                drop(previous_value.assume_init());
            }
            self.read_index += 1;
        }
        let index = self.write_index % CAP;
        self.buffer[index] = MaybeUninit::new(value);
        self.write_index += 1;
    }

    fn pop_front(&mut self) -> Option<T> {
        if self.is_empty() {
            None
        } else {
            let index = self.read_index % CAP;
            let res = mem::replace(&mut self.buffer[index], MaybeUninit::uninit());
            self.read_index += 1;

            // Safety: elements in buffer are always initialized when inserted,
            // so if the buffer is not empty then the popped element is guaranteed
            // to be initialized
            unsafe { Some(res.assume_init()) }
        }
    }

    fn clear(&mut self) {
        for i in self.drain() {
            drop(i);
        }

        self.read_index = 0;
        self.write_index = 0;
    }

    fn fill_with<F: FnMut() -> T>(&mut self, mut f: F) {
        self.clear();
        self.read_index = 0;
        self.write_index = CAP;
        self.buffer.fill_with(|| MaybeUninit::new(f()));
    }

    fn as_slices(&self) -> (&[T], &[T]) {
        if self.len() == 0 {
            (&[], &[])
        } else if self.write_index > self.read_index {
            let slice_a = &self.buffer[self.write_index..self.len()];
            (unsafe { maybe_uninit_slice_assume_init_ref(slice_a) }, &[])
        } else {
            let slice_a = &self.buffer[self.write_index..self.len()];
            let slice_b = &self.buffer[0..self.read_index];
            (
                unsafe { maybe_uninit_slice_assume_init_ref(slice_a) },
                unsafe { maybe_uninit_slice_assume_init_ref(slice_b) },
            )
        }
    }
}

impl<T, const CAP: usize> Default for FixedRingBuffer<T, CAP> {
    fn default() -> Self {
        Self::new()
    }
}

impl<RB, const CAP: usize> FromIterator<RB> for FixedRingBuffer<RB, CAP> {
    fn from_iter<T: IntoIterator<Item = RB>>(iter: T) -> Self {
        let mut res = Self::default();
        for i in iter {
            res.push_back(i);
        }
        res
    }
}

impl<T, const CAP: usize> Index<usize> for FixedRingBuffer<T, CAP> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        self.get(index).expect("index out of bounds")
    }
}

impl<T, const CAP: usize> IndexMut<usize> for FixedRingBuffer<T, CAP> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        self.get_mut(index).expect("index out of bounds")
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
        let i = rb.pop_front();
        assert_eq!(i, None);
    }

    #[test]
    fn push_single() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        rb.push_back(42);
        assert_eq!(rb.len(), 1);
        assert_eq!(rb.is_empty(), false);
        assert_eq!(rb.is_full(), false);
    }

    #[test]
    fn push_n() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=10 {
            rb.push_back(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
            assert_eq!(rb.is_full(), false);
        }
    }
    #[test]
    fn push_n_full() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=16 {
            rb.push_back(i);
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
            rb.push_back(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
            if i == 16 {
                assert_eq!(rb.is_full(), true);
            } else {
                assert_eq!(rb.is_full(), false);
            }
        }
        for i in 17..=22 {
            rb.push_back(i);
            assert_eq!(rb.len(), rb.capacity());
            assert_eq!(rb.is_empty(), false);
            assert_eq!(rb.is_full(), true);
        }
    }

    #[test]
    fn iter_n() {
        let mut rb = FixedRingBuffer::<i32, 16>::new();
        for i in 1..=8 {
            rb.push_back(i);
            assert_eq!(rb.len(), i as usize);
            assert_eq!(rb.is_empty(), false);
        }
        for (i, element) in rb.iter().enumerate() {
            assert_eq!(*element, (i + 1) as i32);
        }
    }
}
