package com.phicomm.decoder;

import android.os.StatFs;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private Button codec;
    private TextView tv_info;
    private Button btnDecoder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
    }

    private void init() {

        codec = (Button) findViewById(R.id.btn_codec);
        tv_info= (TextView) findViewById(R.id.tv_info);
        btnDecoder= (Button) findViewById(R.id.button);
        codec.setOnClickListener(this);
        btnDecoder.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {

            case R.id.btn_codec:
                //tv_info.setText(avcodecinfo());
                break;

            case R.id.button:
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        startDecode();
                    }
                }).start();
                break;

            default:
                break;
        }
    }

    private void startDecode() {

        testvideos();


    }
    private void testaudiosandvideos(){
        String dir = "/storage/emulated/0/testmusic";
        File dirfile =new File(dir);
        for(File file: dirfile.listFiles()){
            String inputurl = file.getAbsolutePath();
            String outputurl = file.getParent() + "/zxtmp/";
            File destfile = new File(outputurl);
            if (!destfile.exists() || !destfile.isDirectory()) {
                destfile.mkdirs();
            }
            outputurl = destfile.getPath() + File.separator+"thumb"+file.getName()+".jpg";
            long t1 = System.currentTimeMillis();
            Log.e("ws-----------inputurl", inputurl);
            Log.e("ws------------outputurl", outputurl);
            Log.d("Decode","result:"+ DecodeFrameUtil.randomscreen(inputurl, outputurl)+",time cost :"+(System.currentTimeMillis()-t1));
        }
    }

    private void testvideos(){
        String [] testPaths={
                "/storage/emulated/0/0908__HDR.mp4",
                "/storage/emulated/0/3gp.3gp",
                "/storage/emulated/0/3g2.3g2",
                "/storage/emulated/0/amv.amv",
                "/storage/emulated/0/avi.avi",
                "/storage/emulated/0/flv.flv",
                "/storage/emulated/0/mkv.mkv",
                "/storage/emulated/0/mov.mov",
                "/storage/emulated/0/mp4.mp4",
                "/storage/emulated/0/mpg.mpg",
                "/storage/emulated/0/mtv.mtv",
                "/storage/emulated/0/rmvb.rmvb",
                "/storage/emulated/0/swf.swf",
                "/storage/emulated/0/webm.webm",
                "/storage/emulated/0/wmv.wmv",
                "/storage/emulated/0/梁静茹_别人的天长地久.wmv",
                "/storage/emulated/0/梁静茹_孤单被半球.wmv",
                "/storage/emulated/0/测试视频原图.mp4",
                "/storage/emulated/0/稻香.wmv",
                "/storage/emulated/0/FactoryTest.mkv",
                "/storage/emulated/0/VID_20180417.3gp"
        };

        for(String path :testPaths){
            File file = new File(path);
            String inputurl = file.getAbsolutePath();
            String outputurl = file.getParent() + "/zxctmp/";
            File destfile = new File(outputurl);
            if (!destfile.exists() || !destfile.isDirectory()) {
                destfile.mkdirs();
            }
            outputurl = destfile.getPath() + File.separator+"thumb"+file.getName()+".jpg";
            long t1 = System.currentTimeMillis();
            Log.e("ws-----------inputurl", inputurl);
            Log.e("ws------------outputurl", outputurl);
            Log.d("Decode","result:"+ DecodeFrameUtil.randomscreen(inputurl, outputurl)+",time cost :"+(System.currentTimeMillis()-t1));

        }
    }
    private void testsimplefile(){
               /* File file = new File("/storage/emulated/0/0908__HDR.mp4");
        String inputurl = file.getAbsolutePath();
        String outputurl = file.getParent() + "/zxtmp/";
        File destfile = new File(outputurl);
        if (!destfile.exists() || !destfile.isDirectory()) {
            destfile.mkdirs();
        }
        outputurl = destfile.getPath() + "/thumb_0908__HDR_test.jpg";
        Log.e("ws-----------inputurl", inputurl);
        Log.e("ws------------outputurl", outputurl);
        Log.d("Decode","frame count:"+ DecodeFrameUtil.randomscreen(inputurl, outputurl));*/
    }

    public static String getRomAvalibleSize(String path) {
        StatFs mStatFs = new StatFs(path);
        long mBlockSize = mStatFs.getBlockSize();
        long mAvailbaleCount = mStatFs.getAvailableBlocks();
        return String.format(Locale.ENGLISH, "%.2f", mAvailbaleCount * mBlockSize / 1024.0 / 1024.0 / 1024.0) + "G";
    }

}
