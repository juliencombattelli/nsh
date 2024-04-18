//! A queue implemented with a fixed-capacity ring buffer allocated on the stack.

use core::iter::FromIterator;
use core::mem;
use core::mem::MaybeUninit;
use core::ops::{Index, IndexMut};

pub struct ConstGenericRingBuffer<T, const CAP: usize> {
    buf: [MaybeUninit<T>; CAP],
    readptr: usize,
    writeptr: usize,
}
