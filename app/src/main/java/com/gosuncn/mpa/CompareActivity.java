package com.gosuncn.mpa;

import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.gosuncn.mpa.databinding.ActivityCompareBinding;
import com.gosuncn.mplay.RtspPlayer;

/**
 * 播放器对比
 */
public class CompareActivity extends AppCompatActivity {

    ActivityCompareBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = DataBindingUtil.setContentView(this, R.layout.activity_compare);
        init();
    }

    private void init() {
        binding.etRtspUrl.setText("rtsp://192.168.16.135/video/264/tc10.264");
        binding.rpVideo.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading.setVisibility(View.GONE);
                Toast.makeText(CompareActivity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();
            }
        });
        binding.rpVideo2.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code, String msg) {
                binding.pbLoading2.setVisibility(View.GONE);
                Toast.makeText(CompareActivity.this, code == 1 ? "播放成功" : msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void onPlayClick(View view) {
        binding.pbLoading.setVisibility(View.VISIBLE);
        binding.rpVideo.startPlay(binding.etRtspUrl.getText().toString());
        binding.pbLoading2.setVisibility(View.VISIBLE);
        binding.rpVideo2.startPlay(binding.etRtspUrl.getText().toString());
    }



    public void onStopClick(View view) {
        binding.pbLoading.setVisibility(View.GONE);
        binding.rpVideo.stopPlay();
        binding.pbLoading2.setVisibility(View.GONE);
        binding.rpVideo2.stopPlay();
    }
}
