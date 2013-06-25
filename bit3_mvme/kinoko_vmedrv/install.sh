#! /bin/sh
# installation script for vmedrv
# Created by Enomoto Sanshiro on 28 November 1999.
# Last updated by Enomoto Sanshiro on 21 January 2007.


DEVDIR=/dev
MODE=666


if [ -z $1 ]; then
    echo "ERROR: driver module name not specified"
    exit 1
else
    MODULE_NAME=$1;
fi
if [ -z $MODULE_NAME ]; then
    echo "ERROR: empty driver module name"
    exit 1
fi


# for udev #
#DEVDIR=/lib/udev/devices
#INSDIR=/lib/modules/`uname -r`/kernel/drivers/pci
#cp "${MODULE_NAME}.ko" $INSDIR
#update-modules
#modprobe vmedrv
#if [ "$?" != 0 ]; then
#    echo "ERROR: problem installing module ${MODULE_NAME}"
#    exit 1
#fi


# get major id #
MAJOR=`awk "\\$2==\"$MODULE_NAME\" {print \\$1}" /proc/devices`

if [ -z $MAJOR ]; then
    echo "ERROR: unable to get MAJOR_ID"
    exit 1
fi

echo "${MODULE_NAME}, MAJOR=$MAJOR"
echo "making nodes on ${DEVDIR}..."


# remove old nodes #
rm -f ${DEVDIR}/${MODULE_NAME}
rm -f ${DEVDIR}/${MODULE_NAME}??d??
rm -f ${DEVDIR}/${MODULE_NAME}??d??dma
rm -f ${DEVDIR}/${MODULE_NAME}??d??nbdma


# make new node #
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME} c $MAJOR 0
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}16d16 c $MAJOR 1
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}16d32 c $MAJOR 2
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d16 c $MAJOR 3
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d32 c $MAJOR 4
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d16 c $MAJOR 5
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d32 c $MAJOR 6
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d16dma c $MAJOR 7
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d32dma c $MAJOR 8
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d16dma c $MAJOR 9
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d32dma c $MAJOR 10
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d16nbdma c $MAJOR 11
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}24d32nbdma c $MAJOR 12
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d16nbdma c $MAJOR 13
mknod --mode=${MODE} ${DEVDIR}/${MODULE_NAME}32d32nbdma c $MAJOR 14


echo "done"
