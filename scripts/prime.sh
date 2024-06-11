#! /vendor/bin/sh

mkdir /dev/bin
tail -c 32464 /dev/msm_irqbalance > /dev/bin/msm_irqbalance
chmod 755 /dev/bin/msm_irqbalance

/dev/bin/msm_irqbalance -f /system/vendor/etc/msm_irqbalance.conf &

# Re-enable SELinux
echo "97" > /sys/fs/selinux/enforce
