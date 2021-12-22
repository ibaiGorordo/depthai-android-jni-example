package com.example.depthai_android_jni_example;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.depthai_android_jni_example.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'depthai_android_jni_example' library on application startup.
    static {
        System.loadLibrary("depthai_android_jni_example");
    }

    private ImageView rgbImageView, depthImageView;
    private Bitmap rgb_image, depth_image;

    private static final int rgbWidth = 640;
    private static final int rgbHeight = 480;
    private static final int disparityWidth = 640;
    private static final int disparityHeight = 400;
    private static final int framePeriod = 30;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        com.example.depthai_android_jni_example.databinding.ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Initialize the image views to show the images
        rgbImageView = binding.rgbImageView;
        depthImageView = binding.depthImageView;

        // Initialize the image bitmaps to store the image data
        rgb_image = Bitmap.createBitmap(rgbWidth, rgbHeight, Bitmap.Config.ARGB_8888);
        depth_image = Bitmap.createBitmap(disparityWidth, disparityHeight, Bitmap.Config.ARGB_8888);

        // Start the device
        startDevice(rgbWidth, rgbHeight);

        // Main loop that will read the feed from the device at framePeriod (ms) intervals
        runnable.run();
    }

    // Main loop where the data is obtained from the device and shown into the screen
    private final Handler handler = new Handler();
    private final Runnable runnable = new Runnable() {
        public void run() {

            rgb_image.setPixels(imageFromJNI(), 0, rgbWidth, 0, 0, rgbWidth, rgbHeight);
            rgbImageView.setImageBitmap(rgb_image);

            depth_image.setPixels(depthFromJNI(), 0, disparityWidth, 0, 0, disparityWidth, disparityHeight);
            depthImageView.setImageBitmap(depth_image);

            handler.postDelayed(this, framePeriod);
        }
    };


    /**
     * A native method that is implemented by the 'depthai_android_jni_example' native library,
     * which is packaged with this application.
     */
    public native void startDevice(int rgbWidth, int rgbHeight);
    public native int[] imageFromJNI();
    public native int[] depthFromJNI();
}