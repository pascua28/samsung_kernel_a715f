#! /vendor/bin/sh

mkdir /dev/bin
tail -c 32464 /dev/msm_irqbalance > /dev/bin/msm_irqbalance
chmod 755 /dev/bin/msm_irqbalance

cp /vendor/etc/init/hw/init.qcom.rc /dev/init.qcom.rc
sed -i -e 's/\/vendor\/bin\/msm_irqbalance/\/dev\/bin\/msm_irqbalance/g' /dev/init.qcom.rc
mount --bind /dev/init.qcom.rc /vendor/etc/init/hw/init.qcom.rc

# Re-enable SELinux
echo "97" > /sys/fs/selinux/enforce
