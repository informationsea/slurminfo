#include "assoc.h"
#include <slurm/slurm.h>
#include <slurm/slurmdb.h>
#include <memory.h>

void print_assoc(FILE *file)
{
    assoc_mgr_info_request_msg_t req;
    memset(&req, 0, sizeof(req));
    req.flags = ASSOC_MGR_INFO_FLAG_ASSOC | ASSOC_MGR_INFO_FLAG_USERS;

    assoc_mgr_info_msg_t *result;
    int ret = slurm_load_assoc_mgr_info(&req, &result);
    if (ret != SLURM_SUCCESS)
    {
        fprintf(stderr, "Failed to load association information: %s\n", slurm_strerror(ret));
        return;
    }
    fprintf(file, "ACCOUNT_NAME\tID\tPARENT_ID\tGRP_TRES\tUSER\tUID\tIS_DEFAULT\n");

    ListIterator iter = slurm_list_iterator_create(result->assoc_list);
    slurmdb_assoc_rec_t *assoc;
    while ((assoc = slurm_list_next(iter)))
    {
        fprintf(file, "%s\t%d\t%u\t%s\t%s\t%u\t%u\n",
                assoc->acct, assoc->id, assoc->parent_id, assoc->grp_tres,
                assoc->user, assoc->uid, assoc->is_def);
    }
    slurm_list_iterator_destroy(iter);
    slurm_free_assoc_mgr_info_msg(result);
}