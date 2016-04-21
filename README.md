Android Ads and "Ad-Free" Purchasing Integration for Orx
========================================================

This folder contains the source files needed to integrate into an Orx game AdMob ads and an in-app purchasable item called "AdFree" to remove ads. The files are not usable as drop-in as you'll have to integrate the flow into your game, but, hopefully, following this readme should let you do that easily. My intention was to package these as a nice library, but I don't have the time to do that right now, so there you go :)

##Platform
What we call "platform" in this package is an abstraction that lets your game communicate with the platform it's running on. The `Platform` class is an abstract base that should be implemented separately for each platform.

`platform.h` contains the definition of the `Platform` class which lets the game call functions to be handled by the platform. `platform.h` also contains some free functions which will be called by the platform to tell various things to the game. **Note!** This means that the methods of the `Platform` class are meant to be called from the Orx thread, while the free functions in `platform.h` will typically be called from another thread (Android UI thread for example).

`platform.cpp` just contains the implementations of the Get/Set platform functions, nothing fancy.

`platform.i` is a SWIG interface definition file. You can generate the `platform_wrapper.cpp/h` files and the files under the `android_src_main_java/platform` folder by feeding this file to SWIG. I've included the generated files in this folder for convenience, but if you want to add new capabilities to your platform, you can use this interface file.

##Monetization
`monetization.cpp/h` contains the code to integrate the ad workflow into your game. All one needs to do is implement the various commented places in `monetization.cpp` call the `init/update/exit_monetization` functions at relevant times, and call the `RequestTransition` function whenever the game is in a suitable state to show Ads (you should keep calling it every frame until it returns true).

##Create The Ad and The AdFree Item
Now, on your AdMob account, create an Ad and note down its ID. Then on your Google Play Developer account, create an in-app-billing item called "adfree" and note down your game's license key.

##Java Side
On the java side, one needs to do the following:

Include `android_src_main_java/*` As well as `AndroidPlatform.java` with your java sources and just put `android_src_main/*` under your `src/main` folder. Add the following to your MainActivity:


``` java
public class MainActivity extends OrxActivity {

    // ... Other stuff

    AndroidPlatform platform;
    IabHelper mHelper;
    static final int PURCHASE_REQUEST_CODE = 1;

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == PURCHASE_REQUEST_CODE) mHelper.handleActivityResult(requestCode, resultCode, data);
    }

    protected void onCreate(Bundle savedInstanceState) {

        // ... Other stuff
        String license_key = IntensionalCompileError!!! // Your game license key here (Get it from your Google Play Developer Console)
        mHelper = new IabHelper(context, license_key);

        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    Log.d("Billing", "Problem setting up In-app Billing: " + result);
                }

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null) return;
                platform.IABSetupSuccessful(mHelper);
            }
        });        

        platform = new AndroidPlatform(this);
        PlatformModule.SetPlatform(platform);
    
        // ... Other stuff

    }

    // ... Other stuff
}
```

You also need to add the following to your `AndroidManifest.xml`
``` xml
<manifest ...>
    ...
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
    <uses-permission android:name="com.android.vending.BILLING" />
</manifest>
```

Then, add the foloowing to your `build.gradle`
``` groovy
dependencies {
    ...
    compile 'com.android.support:support-v4:20.0.0'
    compile 'com.google.android.gms:play-services-ads:8.4.0'
}
```
This means you also need to install the `support` and `play-services-ads` packages in your Android package manager.

##Conclusion
I hope that's all, if this procedure fails somehow, let me know.
