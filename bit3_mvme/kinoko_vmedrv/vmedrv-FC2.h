

#define ioread8(address) readb(address)
#define ioread16(address) readw(address)
#define ioread32(address) readl(address)

#define iowrite8(value, address) writeb((value), (address))
#define iowrite16(value, address) writew((value), (address))
#define iowrite32(value, address) writel((value), (address))

#define remap_pfn_range(vma, start, pfn, size, prot) remap_page_range((vma), (start), (pfn) << PAGE_SHIFT, (size), (prot))
