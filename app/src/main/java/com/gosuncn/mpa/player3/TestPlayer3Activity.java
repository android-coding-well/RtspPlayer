package com.gosuncn.mpa.player3;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.gosuncn.mpa.R;
import com.gosuncn.mpa.databinding.ActivityTestPlayer3Binding;
import com.gosuncn.mplay.RtspPlayer;

import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

public class TestPlayer3Activity extends AppCompatActivity {

    ActivityTestPlayer3Binding binding;
    String dir = Environment.getExternalStorageDirectory().toString() + "/RtspPlayer";

    Timer timer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding= DataBindingUtil.setContentView(this,R.layout.activity_test_player3);

        //binding.etRtspUrl.setText("rtsp://192.168.16.135/video/264/tc10.264");
       // binding.etRtspUrl.setText(Environment.getExternalStorageDirectory().toString() + "/test.mp4");
        binding.etRtspUrl.setText("rtsp://192.168.16.135/video/aac/winamp-intro.aac");
        binding.etRtspUrl.setText("rtsp://192.168.16.135/video/aac/96.mp3");
        binding.etRtspUrl.setText("rtsp://192.168.16.135/video/264/test.mkv");
       // binding.etRtspUrl.setText("rtsp://192.168.16.135/video/264/test.264");
        //binding.etRtspUrl.setText("rtsp://192.168.15.64/b1.avi");
        //binding.etRtspUrl.setText("rtsp://192.168.15.64/m1.avi");

        binding.rpVideo.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading.setVisibility(View.GONE);
                Toast.makeText(TestPlayer3Activity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();

            }
        });
    }

    public void onPlay1Click(View view) {
        //binding.tbtnSound1.setChecked(false);
        binding.pbLoading.setVisibility(View.VISIBLE);
        binding.rpVideo.startPlay(binding.etRtspUrl.getText().toString());
        timer=new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        binding.tvFrame1.setText(binding.rpVideo.getReceiveVideoCount()+" "+binding.rpVideo.getReceiveAudioCount());
                        binding.tvDecode1.setText(binding.rpVideo.getDecodeVideoCount()+" "+binding.rpVideo.getDecodeAudioCount());
                        binding.tvDecodeFailed1.setText(binding.rpVideo.getDecodeFailedVideoCount()+" "+binding.rpVideo.getDecodeFailedAudioCount());
                        binding.tvRender1.setText(""+binding.rpVideo.getRenderCount()+" "+binding.rpVideo.getPlayAudioCount());
                    }
                });


            }
        },100,25);
    }


    public void onStop1Click(View view) {
        binding.tbtnSound1.setChecked(false);
        binding.pbLoading.setVisibility(View.GONE);
        binding.rpVideo.stopPlay();
        if(timer!=null){
            timer.cancel();
            timer=null;
        }

    }

    public void onCapture1Click(View view) {
        final String path = dir + "/test/rtsp3_" + new Date().getTime() + ".png";
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},0);
            Toast.makeText(this, "请允许写权限", Toast.LENGTH_SHORT).show();
            return;
        }
        Toast.makeText(this, binding.rpVideo.capture(path) == 1 ? "抓拍成功" : "抓拍失败", Toast.LENGTH_SHORT).show();
    }

    public void onSound1Click(View view) {
        if (binding.tbtnSound1.isChecked()) {
            binding.rpVideo.openSound();
        } else {
            binding.rpVideo.closeSound();
        }
    }

    public void onFourScreenClick(View view) {
        Intent intent = new Intent(this, TestFourPlayer3Activity.class);
        startActivity(intent);
    }
}
