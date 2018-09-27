# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in /Users/cvanes/Dev/Tools/android-sdk-macosx/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

-keep class co.instil.surge.authentication.** {*;}
-keep class co.instil.surge.callbacks.** {*;}
-keep class co.instil.surge.client.** {*;}
-keep class co.instil.surge.decoders.** {*;}
-keep class co.instil.surge.device.** {*;}
-keep class co.instil.surge.player.** {*;}
-keepclassmembers class co.instil.surge.authentication.** {*;}
-keepclassmembers class co.instil.surge.callbacks.** {*;}
-keepclassmembers class co.instil.surge.client.** {*;}
-keepclassmembers class co.instil.surge.decoders.** {*;}
-keepclassmembers class co.instil.surge.device.** {*;}
-keepclassmembers class co.instil.surge.player.** {*;}