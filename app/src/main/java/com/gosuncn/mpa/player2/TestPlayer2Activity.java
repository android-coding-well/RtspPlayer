package com.gosuncn.mpa.player2;

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
import com.gosuncn.mpa.databinding.ActivityTestPlayer2Binding;
import com.gosuncn.mplay.RtspPlayer;

import java.util.Date;

public class TestPlayer2Activity extends AppCompatActivity {

    ActivityTestPlayer2Binding binding;
    String dir = Environment.getExternalStorageDirectory().toString() + "/RtspPlayer";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = DataBindingUtil.setContentView(this, R.layout.activity_test_player2);
        binding.etRtspUrl.setText("rtsp://192.168.39.19/video/264/tc10.264");

        binding.rpVideo.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading.setVisibility(View.GONE);
                Toast.makeText(TestPlayer2Activity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void onPlay1Click(View view) {
        binding.pbLoading.setVisibility(View.VISIBLE);
        binding.rpVideo.startPlay(binding.etRtspUrl.getText().toString());
    }


    public void onStop1Click(View view) {
        binding.pbLoading.setVisibility(View.GONE);
        binding.rpVideo.stopPlay();
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


    public void onSound1Click(View view) {
        if (binding.tbtnSound1.isChecked()) {
            binding.rpVideo.openSound();
        } else {
            binding.rpVideo.closeSound();
        }
    }

    public void onFourScreenClick(View view) {
        Intent intent = new Intent(this, TestFourScreenActivity.class);
        startActivity(intent);
    }

}
