#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <liboffsetfinder64/kernelpatchfinder64.hpp>
#include <liboffsetfinder64/machopatchfinder64.hpp>

#define HAS_ARG(x, y) (!strcmp(argv[i], x) && (i + y) < argc)

using namespace tihmstar::offsetfinder64;

#define FLAG_ONLY_AMFI (1 << 0)

int main(int argc, const char *argv[])
{
    FILE *fp = NULL;
    int flags = 0;

    if (argc < 3)
    {
        printf("Usage: %s <kernel_in> <kernel_out> [args]\n", argv[0]);
        printf("\t-o \t\tAMFI patch only.\n");
        return -1;
    }

    printf("%s: Starting...\n", __FUNCTION__);

    for (int i = 0; i < argc; i++)
    {
        if (HAS_ARG("-o", 0))
        {
            flags |= FLAG_ONLY_AMFI;
        }
    }

    std::vector<patch> patches;
    kernelpatchfinder64 kpf(argv[1]);

    try
    {
        printf("Kernel: Adding AMFI patch...\n");
        auto p = kpf.get_amfi_patch();
        patches.insert(patches.end(), p.begin(), p.end());
    }
    catch (...)
    {
        printf("Error:doing get_amfi_patch()!\n");
        return -1;
    }

    /* Jailbreak patch by liboffsetfinder64 */
    try
    {
        printf("Kernel: Adding MarijuanARM patch...\n");
        auto p = kpf.get_MarijuanARM_patch();
        patches.insert(patches.end(), p.begin(), p.end());
    }
    catch (...)
    {
        printf("Failure:MarijuanARM patch\n");
    }

    if (flags == 0)
    {
        try
        {
            printf("Kernel: Adding task_conversion_eval patch...\n");
            auto p = kpf.get_task_conversion_eval_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:task_conversion_eval patch\n");
        }

        try
        {
            printf("Kernel: Adding vm_fault_internal patch...\n");
            auto p = kpf.get_vm_fault_internal_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:vm_fault_internal patch\n");
        }

        try
        {
            printf("Kernel: Adding trustcache_true patch...\n");
            auto p = kpf.get_trustcache_true_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:trustcache_true patch\n");
        }

        try
        {
            printf("Kernel: Adding mount patch...\n");
            auto p = kpf.get_mount_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:mount patch\n");
        }

        try
        {
            printf("Kernel: Adding tfp0 patch...\n");
            auto p = kpf.get_tfp0_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:tfp0 patch\n");
        }

        try
        {
            printf("Kernel: Adding get_task_allow patch...\n");
            auto p = kpf.get_get_task_allow_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:get_task_allow patch\n");
        }

        try
        {
            printf("Kernel: Adding apfs_snapshot patch...\n");
            auto p = kpf.get_apfs_snapshot_patch();
            patches.insert(patches.end(), p.begin(), p.end());
        }
        catch (...)
        {
            printf("Failure:apfs_snapshot patch\n");
        }
    }
    /* Write out the patched file... */
    fp = fopen(argv[2], "wb+");
    if (!fp)
    {
        printf("%s: Unable to open %s!\n", __FUNCTION__, argv[2]);
        return -1;
    }

    for (auto p : patches)
    {
        char *buf = (char *)kpf.buf();
        tihmstar::offsetfinder64::offset_t off = (tihmstar::offsetfinder64::offset_t)((const char *)kpf.memoryForLoc(p._location) - buf);
        printf("applying patch=%p : ", p._location);
        for (int i = 0; i < p._patchSize; i++)
        {
            printf("%02x", ((uint8_t *)p._patch)[i]);
        }
        printf("\n");
        memcpy(&buf[off], p._patch, p._patchSize);
    }

    printf("%s: Writing out patched file to %s...\n", __FUNCTION__, argv[2]);
    fwrite(kpf.buf(), kpf.bufSize(), 1, fp);

    fflush(fp);
    fclose(fp);

    printf("%s: Quitting...\n", __FUNCTION__);

    return 0;
}
