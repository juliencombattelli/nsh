use core::fmt;

trait AsStr {
    fn as_bytes(&self) -> &[u8];
    unsafe fn as_bytes_mut(&mut self) -> &mut [u8];
    fn new() -> Self;
    fn capacity(&self) -> usize;
    #[inline(always)]
    fn is_empty(&self) -> bool {
        self.len() == 0
    }
    #[inline(always)]
    fn len(&self) -> usize {
        self.as_bytes().len()
    }
}

#[derive(Debug, Copy, Clone, PartialEq)]
pub(crate) struct FixedString<const N: usize> {
    bytes: [u8; N],
    len: u16, // makes struct grow in steps of 2, unlike usize, which starts at 16 bytes and grows by 8
}
impl<const N: usize> AsStr for FixedString<N> {
    #[inline(always)]
    fn as_bytes(&self) -> &[u8] {
        &self.bytes[..self.len as usize]
    }
    #[inline(always)]
    unsafe fn as_bytes_mut(&mut self) -> &mut [u8] {
        &mut self.bytes[..self.len as usize]
    }
    #[inline(always)]
    fn new() -> Self {
        Self {
            bytes: [0_u8; N],
            len: 0,
        }
    }
    #[inline(always)]
    fn capacity(&self) -> usize {
        N
    }
}
impl<const N: usize> From<&str> for FixedString<N> {
    fn from(str: &str) -> Self {
        let mut new = Self {
            // This should be SAFE, as long as all methods extend contiguously and never index beyond len
            // However clippy doesn't like it... It's more efficient though, than 1st zeroing N bytes, and then copying over it.
            bytes: unsafe { std::mem::MaybeUninit::<[u8; N]>::uninit().assume_init() },
            len: str.len() as u16,
        };
        new.bytes[..str.len()].copy_from_slice(str.as_bytes());
        new
    }
}
impl<const N: usize> AsRef<str> for FixedString<N> {
    // why is this not considered when assigning to &str?
    fn as_ref(&self) -> &str {
        // How to do this better? Clippy warns: deref on an immutable reference
        std::str::from_utf8(&(*self.as_bytes())).expect("bad string")
    }
}
impl<const N: usize> fmt::Display for FixedString<N> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.as_ref())
    }
}
