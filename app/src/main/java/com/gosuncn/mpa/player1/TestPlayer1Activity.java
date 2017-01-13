package com.gosuncn.mpa.player1;

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
import com.gosuncn.mpa.databinding.ActivityTestPlayer1Binding;
import com.gosuncn.mplay.RtspPlayer;

import java.util.Date;

public class TestPlayer1Activity extends AppCompatActivity {

    //rtsp://1:1@192.168.15.65:554/xms/realplay/44011201/8/35?streams=0
    //rtsp://1:1@192.168.28.17:554/xms/realplay/44011201/8/35?streams=0
    //rtsp://192.168.28.29:8554/2.264
    //rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp
    private static final String TAG = "TestPlayer1Activity";
    ActivityTestPlayer1Binding binding;
    String dir = Environment.getExternalStorageDirectory().toString() + "/RtspPlayer";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = DataBindingUtil.setContentView(this, R.layout.activity_test_player1);

        binding.etRtspUrl.setText("rtsp://192.168.16.135/video/264/tc10.264");
        binding.etRtspUrl2.setText(Environment.getExternalStorageDirectory().toString() + "/test.mp4");

        binding.rpVideo.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading.setVisibility(View.GONE);
                Toast.makeText(TestPlayer1Activity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();
            }
        });
        binding.rpVideo2.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading2.setVisibility(View.GONE);
                Toast.makeText(TestPlayer1Activity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void onPlay1Click(View view) {
        binding.pbLoading.setVisibility(View.VISIBLE);
        binding.rpVideo.startPlay(binding.etRtspUrl.getText().toString());
    }

    public void onPlay2Click(View view) {
        binding.pbLoading2.setVisibility(View.VISIBLE);
        binding.rpVideo2.startPlay(binding.etRtspUrl2.getText().toString());
    }

    public void onStop1Click(View view) {
        binding.pbLoading.setVisibility(View.GONE);
        binding.rpVideo.stopPlay();
    }

    public void onStop2Click(View view) {
        binding.pbLoading2.setVisibility(View.GONE);
        binding.rpVideo2.stopPlay();
    }

    public void onFourScreenClick(View view) {
        Intent intent = new Intent(this, FourScreenActivity.class);
        startActivity(intent);
    }

    public void onCapture1Click(View view) {
        final String path = dir + "/test/rtsp1_" + new Date().getTime() + ".png";
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},0);
            Toast.makeText(this, "请允许写权限", Toast.LENGTH_SHORT).show();
            return;
        }
        Toast.makeText(this, binding.rpVideo.capture(path) == 1 ? "抓拍成功" : "抓拍失败", Toast.LENGTH_SHORT).show();
    }

    public void onCapture2Click(View view) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},0);
            Toast.makeText(this, "请允许写权限", Toast.LENGTH_SHORT).show();
            return;
        }
        final String path = dir + "/rtsp2_" + new Date().getTime() + ".jpg";
        Toast.makeText(this, binding.rpVideo2.capture(path) == 1 ? "抓拍成功" : "抓拍失败", Toast.LENGTH_SHORT).show();
    }

    public void onSound1Click(View view) {
        if (binding.tbtnSound1.isChecked()) {
            binding.rpVideo.openSound();
        } else {
            binding.rpVideo.closeSound();
        }
    }

    public void onSound2Click(View view) {
        if (binding.tbtnSound2.isChecked()) {
            binding.rpVideo2.openSound();
        } else {
            binding.rpVideo2.closeSound();
        }
    }

}
