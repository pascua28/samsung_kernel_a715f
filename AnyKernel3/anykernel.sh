### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers

### AnyKernel setup
# global properties
properties() { '
kernel.string=Prime Kernel by pascua28 @ xda-developers
do.devicecheck=1
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=a71
device.name2=m51
device.name3=
device.name4=
device.name5=
supported.versions=
supported.patchlevels=
supported.vendorpatchlevels=
'; } # end properties


### AnyKernel install
## boot files attributes
attributes() {
set_perm_recursive 0 0 755 644 $ramdisk/*;
set_perm_recursive 0 0 750 750 $ramdisk/init* $ramdisk/sbin;
} # end attributes

# boot shell variables
block=/dev/block/bootdevice/by-name/boot;
is_slot_device=0;
ramdisk_compression=auto;
patch_vbmeta_flag=auto;

# import functions/variables and setup patching - see for reference (DO NOT REMOVE)
. tools/ak3-core.sh;

# boot install
dump_boot;

oneui=$(file_getprop /system/build.prop ro.build.version.oneui);

#if [ $oneui == "60000" ]; then
#   ui_print ""
#   ui_print "OneUI 6.0 detected! Patching selinux"
#   patch_cmdline "androidboot.selinux" "androidboot.selinux=permissive";
#fi

oneui=$(file_getprop /system/build.prop ro.build.version.oneui);
device=$(file_getprop /system/build.prop ro.product.system.device);

if [ -n "$oneui" ]; then
   ui_print "OneUI ROM detected!"
   patch_cmdline "android.is_aosp" "android.is_aosp=0";
elif [ $device == "generic" ]; then
   ui_print "GSI ROM detected!"
   patch_cmdline "android.is_aosp" "android.is_aosp=0";
else
   ui_print "AOSP ROM detected!"
   patch_cmdline "android.is_aosp" "android.is_aosp=1";
fi

write_boot;
## end boot install
