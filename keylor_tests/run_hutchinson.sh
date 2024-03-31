#!/bin/bash
#SBATCH --job-name=keylor_chroma_test
#SBATCH --partition=nukwa
#SBATCH --output=npr_test_load_prop.txt
#SBATCH --nodelist=nukwa-05.cnca
#SBATCH --ntasks=1
#SBATCH --time=04:00:00


#source /work/karley/ChromaScriptsKabre/cuda-jit/env.sh
source /work/hmonge/compartida/ChromaScriptsKabre/cuda-jit/env.sh


NPROC=1
GEOM="1 1 1 1"
IOGEOM="1 1 1 1"



PROGDIR=/work/karley/ChromaScriptsKabre/cuda-jit/install/chroma/bin
PROGDIR=/work/hmonge/compartida/ChromaScriptsKabre/cuda-jit/install/chroma_ghk/bin
PROG="${PROGDIR}/chroma -geom ${GEOM}"

export QUDA_RESOURCE_PATH=${PROGDIR}
export QUDA_PROFILE_OUTPUT_BASE=profile_64

## For GPU Direct set QUDA_ENABLE_GDR=1 and GPUDIRECT=" -gpudirect "
export QUDA_ENABLE_GDR=1
export GPUDIRECT=" -gpudirect "
#export GPUDIRECT=""

export MPICH_ENV_DISPLAY=1
export MPICH_GPU_SUPPORT_ENABLED=1
export MPICH_COLL_SYNC=MPI_Bcast
export MPICH_OFI_NIC_VERBOSE=1
export OMP_NUM_THREADS=4
export MPICH_SMP_SINGLE_COPY_MODE=NONE
ulimit -c 0


#ini=./npr_hmc.ini.xml
#out=./npr_hmc.out.xml
#stdout=./npr_hmc.stdout

##################################
##################################
#############KEYLOR###############
##################################
##################################

ini=./hutchinson.ini.xml
out=./hutchinson.out.xml
stdout=./hutchinson.stdout

RUN="srun  ${PROG} -i $ini -o $out > $stdout 2>&1"

echo "$RUN"
#$RUN
srun  ${PROG} -i $ini -o $out > $stdout 2>&1
#run ./build_kokkos_kabre.sh > build_out_1.sh 2>&1


