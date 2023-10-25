#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>

#include <slurm/slurm.h>
#include <slurm/slurmdb.h>
#include <slurm/slurm_errno.h>

int get_username(char *buf, size_t buflen) {
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    if (pwd) {
	strncpy(buf, pwd->pw_name, buflen - 1);
	return 0;
    }
    return -1;
}

int get_slurm_account(char *buf, size_t buflen) {
    char username[200];
    get_username(username, sizeof(username));
    
    List users_list = slurm_list_create(NULL);
    slurm_list_append(users_list, (void *)username);
    
    assoc_mgr_info_request_msg_t req = {
	.acct_list = NULL,
	.flags = ASSOC_MGR_INFO_FLAG_USERS | ASSOC_MGR_INFO_FLAG_ASSOC,
	.qos_list = NULL,
	.user_list = users_list
    };

    assoc_mgr_info_msg_t *msg;
    if (slurm_load_assoc_mgr_info(&req, &msg) != SLURM_SUCCESS) {
	perror("Failed to load assoc_mgr");
	    exit (1);
    }


    fprintf(stdout, "assoc_list_count: %d\n", slurm_list_count(msg->assoc_list));
    fprintf(stdout, "qos_list_count: %d\n", slurm_list_count(msg->qos_list));
    fprintf(stdout, "user_list_count: %d\n", slurm_list_count(msg->user_list));
    
    ListIterator it = slurm_list_iterator_create(msg->assoc_list);
    slurmdb_assoc_rec_t *v = NULL;
    while ((v = slurm_list_next(it))) {
	if (v->user && strcmp(v->user, username) == 0) {
	    fprintf(stdout, "%s\n", v->acct);
	    fprintf(stdout, "  %s\n", v->user);
	    fprintf(stdout, "  %s\n", v->grp_tres);

	    ListIterator it2 = slurm_list_iterator_create(msg->assoc_list);
	    slurmdb_assoc_rec_t *v2 = NULL;
	    
	    while ((v2 = slurm_list_next(it2))) {
		if (v2->acct && strcmp(v2->acct, v->acct) == 0) {
		    fprintf(stdout, "  %s\n", v2->grp_tres);
		}
	    }
	}
    }
    
    return 0;
}

void print_account_limit(const char *username, FILE* stdout) {
    List users_list = slurm_list_create(NULL);
    slurm_list_append(users_list, (void *)username);
    
    assoc_mgr_info_request_msg_t req = {
	.acct_list = NULL,
	.flags = ASSOC_MGR_INFO_FLAG_USERS | ASSOC_MGR_INFO_FLAG_ASSOC,
	.qos_list = NULL,
	.user_list = users_list
    };

    assoc_mgr_info_msg_t *msg;
    if (slurm_load_assoc_mgr_info(&req, &msg) != SLURM_SUCCESS) {
	perror("Failed to load assoc_mgr");
	    exit (1);
    }


    fprintf(stdout, "assoc_list_count: %d\n", slurm_list_count(msg->assoc_list));
    fprintf(stdout, "qos_list_count: %d\n", slurm_list_count(msg->qos_list));
    fprintf(stdout, "user_list_count: %d\n", slurm_list_count(msg->user_list));

    ListIterator it_users = slurm_list_iterator_create(msg->user_list);
    slurmdb_user_rec_t *one_user = NULL;
    while ((one_user = slurm_list_next(it_users))) {
	fprintf(stdout, "%s\t%s\n", one_user->name, one_user->default_acct);
	if (one_user->assoc_list != NULL) {
	ListIterator it = slurm_list_iterator_create(one_user->assoc_list);
	slurmdb_assoc_rec_t *v = NULL;
	while ((v = slurm_list_next(it))) {
	    fprintf(stdout, "   %s\t%s\t%s\t%s\t%u\t%u\n", v->acct, v->parent_acct, v->user, v->grp_tres, v->usage->used_jobs, v->usage->used_submit_jobs);
	}
	}
    }

    fprintf(stdout, "\n");
    
    ListIterator it = slurm_list_iterator_create(msg->assoc_list);
    slurmdb_assoc_rec_t *v = NULL;
    while ((v = slurm_list_next(it))) {
	fprintf(stdout, "%s\t%s\t%s\t%s\t%u\t%u\n", v->acct, v->parent_acct, v->user, v->grp_tres, v->usage->used_jobs, v->usage->used_submit_jobs);
    }
    
}
