

#define remap_pfn_range(vma, start, pfn, size, prot) remap_page_range((vma), (start), (pfn) << PAGE_SHIFT, (size), (prot))
