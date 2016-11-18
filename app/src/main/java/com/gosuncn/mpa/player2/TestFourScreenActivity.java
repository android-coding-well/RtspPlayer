package com.gosuncn.mpa.player2;

import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.gosuncn.mpa.R;
import com.gosuncn.mpa.databinding.ActivityTestFourScreenBinding;
import com.gosuncn.mplay.RtspPlayer;

public class TestFourScreenActivity extends AppCompatActivity {
    ActivityTestFourScreenBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        binding = DataBindingUtil.setContentView(this, R.layout.activity_test_four_screen);

        binding.rpPLayer1.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad1.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourScreenActivity.this, "一屏播放成功...", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourScreenActivity.this, "一屏播放失败...", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer2.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad2.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourScreenActivity.this, "二屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourScreenActivity.this, "二屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer3.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad3.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourScreenActivity.this, "三屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourScreenActivity.this, "三屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer4.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad4.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourScreenActivity.this, "四屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourScreenActivity.this, "四屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });

        binding.rpPLayer1.startPlay("rtsp://192.168.28.17/2.264");
        binding.rpPLayer2.startPlay("rtsp://192.168.28.17/2.264");
        binding.rpPLayer3.startPlay("rtsp://192.168.28.17/2.264");
        binding.rpPLayer4.startPlay("rtsp://192.168.28.17/2.264");
    }

    @Override
    protected void onResume() {
        super.onResume();
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {

            }
        },1000);

    }
}
