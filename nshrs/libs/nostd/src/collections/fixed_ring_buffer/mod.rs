//! A queue implemented with a fixed-capacity ring buffer allocated on the stack.
//!
//! Based on https://github.com/NULLx76/ringbuffer

use core::ops::{Index, IndexMut};

pub trait RingBuffer<T>:
    Sized + IntoIterator<Item = T> + Extend<T> + Index<usize, Output = T> + IndexMut<usize>
{
    fn len(&self) -> usize;

    fn is_empty(&self) -> bool {
        self.len() == 0
    }

    fn is_full(&self) -> bool {
        self.len() == self.capacity()
    }

    fn capacity(&self) -> usize;

    fn push_back(&mut self, value: T);

    fn pop_front(&mut self) -> Option<T>;

    fn skip(&mut self) {
        let _ = self.pop_front();
    }

    fn drain(&mut self) -> RingBufferDrainingIterator<T, Self> {
        RingBufferDrainingIterator::new(self)
    }

    fn fill_with<F: FnMut() -> T>(&mut self, f: F);

    fn fill_with_default(&mut self)
    where
        T: Default,
    {
        self.fill_with(Default::default);
    }

    fn fill(&mut self, value: T)
    where
        T: Clone,
    {
        self.fill_with(|| value.clone());
    }

    fn clear(&mut self);


    fn get(&self, index: usize) -> Option<&T>;

    fn get_mut(&mut self, index: usize) -> Option<&mut T>;

    fn peek(&self) -> Option<&T> {
        self.front()
    }

    fn front(&self) -> Option<&T>;

    fn front_mut(&mut self) -> Option<&mut T>;

    fn back(&self) -> Option<&T>;

    fn back_mut(&mut self) -> Option<&mut T>;

    fn iter_mut(&mut self) -> RingBufferMutIterator<T, Self> {
        RingBufferMutIterator::new(self)
    }

    fn iter(&self) -> RingBufferIterator<T, Self> {
        RingBufferIterator::new(self)
    }

    fn contains(&self, elem: &T) -> bool
    where
        T: PartialEq,
    {
        self.iter().any(|i| i == elem)
    }
}

mod iter {
    use super::RingBuffer;
    use core::iter::FusedIterator;
    use core::marker::PhantomData;
    use core::ptr::NonNull;

    pub struct RingBufferIterator<'rb, T, RB: RingBuffer<T>> {
        obj: &'rb RB,
        len: usize,
        index: usize,
        phantom: PhantomData<T>,
    }

    impl<'rb, T, RB: RingBuffer<T>> RingBufferIterator<'rb, T, RB> {
        pub fn new(obj: &'rb RB) -> Self {
            Self {
                obj,
                len: obj.len(),
                index: 0,
                phantom: PhantomData,
            }
        }
    }

    impl<'rb, T: 'rb, RB: RingBuffer<T>> Iterator for RingBufferIterator<'rb, T, RB> {
        type Item = &'rb T;

        fn next(&mut self) -> Option<Self::Item> {
            if self.index < self.len {
                let res = self.obj.get(self.index);
                self.index += 1;
                res
            } else {
                None
            }
        }

        fn size_hint(&self) -> (usize, Option<usize>) {
            (self.len, Some(self.len))
        }
    }

    impl<'rb, T: 'rb, RB: RingBuffer<T>> FusedIterator for RingBufferIterator<'rb, T, RB> {}

    impl<'rb, T: 'rb, RB: RingBuffer<T>> ExactSizeIterator for RingBufferIterator<'rb, T, RB> {}

    impl<'rb, T: 'rb, RB: RingBuffer<T>> DoubleEndedIterator for RingBufferIterator<'rb, T, RB> {
        fn next_back(&mut self) -> Option<Self::Item> {
            if self.len > 0 && self.index < self.len {
                let res = self.obj.get(self.len - 1);
                self.len -= 1;
                res
            } else {
                None
            }
        }
    }

    /// `RingBufferMutIterator` holds a reference to a `RingBuffer` and iterates over it. `index` is the
    /// current iterator position.
    ///
    /// WARNING: NEVER ACCESS THE `obj` FIELD OUTSIDE OF NEXT. It's private on purpose, and
    /// can technically be accessed in the same module. However, this breaks the safety of `next()`
    pub struct RingBufferMutIterator<'rb, T, RB: RingBuffer<T>> {
        obj: NonNull<RB>,
        index: usize,
        len: usize,
        phantom: PhantomData<&'rb mut T>,
    }

    impl<'rb, T, RB: RingBuffer<T>> RingBufferMutIterator<'rb, T, RB> {
        pub fn new(obj: &'rb mut RB) -> Self {
            Self {
                len: obj.len(),
                obj: NonNull::from(obj),
                index: 0,
                phantom: PhantomData,
            }
        }
    }

    impl<'rb, T: 'rb, RB: RingBuffer<T> + 'rb> FusedIterator for RingBufferMutIterator<'rb, T, RB> {}

    impl<'rb, T: 'rb, RB: RingBuffer<T> + 'rb> ExactSizeIterator for RingBufferMutIterator<'rb, T, RB> {}

    impl<'rb, T: 'rb, RB: RingBuffer<T> + 'rb> DoubleEndedIterator
        for RingBufferMutIterator<'rb, T, RB>
    {
        fn next_back(&mut self) -> Option<Self::Item> {
            if self.len > 0 && self.index < self.len {
                self.len -= 1;
                let res = unsafe { RB::ptr_get_mut(self.obj.as_ptr(), self.len) };
                res.map(|i| unsafe { &mut *i })
            } else {
                None
            }
        }
    }

    impl<'rb, T, RB: RingBuffer<T> + 'rb> Iterator for RingBufferMutIterator<'rb, T, RB> {
        type Item = &'rb mut T;

        fn next(&mut self) -> Option<Self::Item> {
            if self.index < self.len {
                let res = unsafe { RB::ptr_get_mut(self.obj.as_ptr(), self.index) };
                self.index += 1;
                // Safety: ptr_get_mut always returns a valid pointer
                res.map(|i| unsafe { &mut *i })
            } else {
                None
            }
        }

        fn size_hint(&self) -> (usize, Option<usize>) {
            (self.len, Some(self.len))
        }
    }

    pub struct RingBufferDrainingIterator<'rb, T, RB: RingBuffer<T>> {
        obj: &'rb mut RB,
        phantom: PhantomData<T>,
    }

    impl<'rb, T, RB: RingBuffer<T>> RingBufferDrainingIterator<'rb, T, RB> {
        pub fn new(obj: &'rb mut RB) -> Self {
            Self {
                obj,
                phantom: PhantomData,
            }
        }
    }

    impl<'rb, T, RB: RingBuffer<T>> Iterator for RingBufferDrainingIterator<'rb, T, RB> {
        type Item = T;

        fn next(&mut self) -> Option<T> {
            self.obj.pop_front()
        }

        fn size_hint(&self) -> (usize, Option<usize>) {
            (self.obj.len(), Some(self.obj.len()))
        }
    }

    pub struct RingBufferIntoIterator<T, RB: RingBuffer<T>> {
        obj: RB,
        phantom: PhantomData<T>,
    }

    impl<T, RB: RingBuffer<T>> RingBufferIntoIterator<T, RB> {
        pub fn new(obj: RB) -> Self {
            Self {
                obj,
                phantom: PhantomData,
            }
        }
    }

    impl<T, RB: RingBuffer<T>> Iterator for RingBufferIntoIterator<T, RB> {
        type Item = T;

        fn next(&mut self) -> Option<Self::Item> {
            self.obj.pop_front()
        }

        fn size_hint(&self) -> (usize, Option<usize>) {
            (self.obj.len(), Some(self.obj.len()))
        }
    }
}

pub use iter::{
    RingBufferDrainingIterator, RingBufferIntoIterator, RingBufferIterator, RingBufferMutIterator,
};

macro_rules! impl_ringbuffer {
    ($readptr: ident, $writeptr: ident) => {
        unsafe fn ptr_len(rb: *const Self) -> usize {
            (*rb).$writeptr - (*rb).$readptr
        }
    };
}

macro_rules! impl_ringbuffer_ext {
    ($get_unchecked: ident, $get_unchecked_mut: ident, $readptr: ident, $writeptr: ident, $mask: expr) => {
        fn get_signed(&self, index: isize) -> Option<&T> {
            use core::ops::Not;
            self.is_empty().not().then(move || {
                let index_from_readptr = if index >= 0 {
                    index
                } else {
                    self.len() as isize + index
                };

                let normalized_index =
                    self.$readptr as isize + index_from_readptr.rem_euclid(self.len() as isize);

                unsafe {
                    // SAFETY: index has been modulo-ed to be within range
                    // to be within bounds
                    $get_unchecked(self, $mask(self.buffer_size(), normalized_index as usize))
                }
            })
        }

        fn get(&self, index: usize) -> Option<&T> {
            use core::ops::Not;
            self.is_empty().not().then(move || {
                let normalized_index = self.$readptr + index.rem_euclid(self.len());
                unsafe {
                    // SAFETY: index has been modulo-ed to be within range
                    // to be within bounds
                    $get_unchecked(self, $mask(self.buffer_size(), normalized_index))
                }
            })
        }

        #[doc(hidden)]
        unsafe fn ptr_get_mut_signed(rb: *mut Self, index: isize) -> Option<*mut T> {
            (Self::ptr_len(rb) != 0).then(move || {
                let index_from_readptr = if index >= 0 {
                    index
                } else {
                    Self::ptr_len(rb) as isize + index
                };

                let normalized_index = (*rb).$readptr as isize
                    + index_from_readptr.rem_euclid(Self::ptr_len(rb) as isize);

                unsafe {
                    // SAFETY: index has been modulo-ed to be within range
                    // to be within bounds
                    $get_unchecked_mut(
                        rb,
                        $mask(Self::ptr_buffer_size(rb), normalized_index as usize),
                    )
                }
            })
        }

        #[doc(hidden)]
        unsafe fn ptr_get_mut(rb: *mut Self, index: usize) -> Option<*mut T> {
            (Self::ptr_len(rb) != 0).then(move || {
                let normalized_index = (*rb).$readptr + index.rem_euclid(Self::ptr_len(rb));

                unsafe {
                    // SAFETY: index has been modulo-ed to be within range
                    // to be within bounds
                    $get_unchecked_mut(rb, $mask(Self::ptr_buffer_size(rb), normalized_index))
                }
            })
        }

        fn clear(&mut self) {
            for i in self.drain() {
                drop(i);
            }

            self.$readptr = 0;
            self.$writeptr = 0;
        }
    };
}

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

    pub fn new<const N: usize>() -> Self {
        let _ = Self::ERROR_CAPACITY_MUST_NOT_BE_ZERO;

        Self {
            buffer: unsafe { MaybeUninit::uninit().assume_init() },
            read_index: 0,
            write_index: 0,
        }
    }
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
    type IntoIter = RingBufferIterator<'a, T, FixedRingBuffer<T, CAP>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a, T, const CAP: usize> IntoIterator for &'a mut FixedRingBuffer<T, CAP> {
    type Item = &'a mut T;
    type IntoIter = RingBufferMutIterator<'a, T, FixedRingBuffer<T, CAP>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

impl<T, const CAP: usize> Extend<T> for FixedRingBuffer<T, CAP> {
    fn extend<A: IntoIterator<Item = T>>(&mut self, iter: A) {
        let iter = iter.into_iter();

        for i in iter {
            self.push_back(i);
        }
    }
}

impl<T, const CAP: usize> RingBuffer<T> for FixedRingBuffer<T, CAP> {
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
            // so if the buffer is not empty then the poped element are garanted
            // to be initialized
            unsafe { Some(res.assume_init()) }
        }
    }

    fn fill_with<F: FnMut() -> T>(&mut self, mut f: F) {
        self.clear();
        self.read_index = 0;
        self.write_index = CAP;
        self.buffer.fill_with(|| MaybeUninit::new(f()));
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
