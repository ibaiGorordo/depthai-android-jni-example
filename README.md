# depthai-android-jni-example (WIP)
 Android example to get the rgb and disparity images from the OAK-D device connected to a phone.
 

https://user-images.githubusercontent.com/43162939/147094587-785c9df2-caa7-40bc-a7dc-c83ed5de41e2.mp4


# Important 
- This example is a simple example to show how to use the depthai library in Android and still it is in progress, so expect bugs. For a proper support of the library in Android, you will have to wait a bit more for the official release.
- Known issue: it is necessary to allow the app to acces the MyriadX (multiple times) every time the app launches. If you have any solutions, please submit a PR.

# License
I have copied directly som of the include folders for some of the dependency libraries. Therefore, all the code except the ones inside the include folder are free to use. I will try to fix it adding submodules in the future.

# References:
- *depthai-core*: https://github.com/luxonis/depthai-core. To build the depthai library for Android, particularly, the *xlink_device_search_improvements* branch.
- *Luxonis Community Discord*: https://discord.gg/tPczgVdk. Check there to get updates and other information about the products from Luxonis.
- *Colormap function*: https://www.particleincell.com/2014/colormap. Used to draw the disparity by modifying the Short Rainbow example.