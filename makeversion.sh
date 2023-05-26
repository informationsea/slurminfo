#!/bin/bash

set -eu -o pipefail

source /etc/os-release

VERSION_NUM=0.2.5
GIT_HASH=$((git rev-parse HEAD||echo 'unknown')|head -c7)
SLURM_VERSION=$(scontrol --version|sed -e 's/slurm //')
OS_VERSION="${ID}-${VERSION_ID}"

SLURMINFO_VERSION="${VERSION_NUM}-${GIT_HASH}-SLURM${SLURM_VERSION}-${OS_VERSION}"

echo -n "${SLURMINFO_VERSION}" > version.txt

cat > version.h <<EOF
#ifndef _VERSION_H
#define _VERSION_H

#define SLURMINFO_VERSION "${SLURMINFO_VERSION}"

#endif
EOF

