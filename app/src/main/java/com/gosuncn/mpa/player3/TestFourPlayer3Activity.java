package com.gosuncn.mpa.player3;

import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.gosuncn.mpa.R;
import com.gosuncn.mpa.databinding.ActivityTestFourPlayer3Binding;
import com.gosuncn.mplay.RtspPlayer;

public class TestFourPlayer3Activity extends AppCompatActivity {
    ActivityTestFourPlayer3Binding binding;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        binding = DataBindingUtil. setContentView(this,R.layout.activity_test_four_player3);

        binding.rpPLayer1.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad1.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourPlayer3Activity.this, "一屏播放成功...", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourPlayer3Activity.this, "一屏播放失败...", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer2.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad2.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourPlayer3Activity.this, "二屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourPlayer3Activity.this, "二屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer3.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad3.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourPlayer3Activity.this, "三屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourPlayer3Activity.this, "三屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });
        binding.rpPLayer4.setOnRtspPlayResultListener(new RtspPlayer.OnRtspPlayResultListener() {
            @Override
            public void onResult(int playerHandle, int code,String msg) {
                binding.pbLoad4.setVisibility(View.GONE);
                if(code==1){
                    Toast.makeText(TestFourPlayer3Activity.this, "四屏播放成功", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(TestFourPlayer3Activity.this, "四屏播放失败", Toast.LENGTH_SHORT).show();
                }
            }
        });

        binding.rpPLayer1.startPlay("rtsp://192.168.16.135/video/264/tc10.264");
        binding.rpPLayer2.startPlay("rtsp://192.168.16.135/video/264/test.mkv");
        binding.rpPLayer3.startPlay("rtsp://192.168.16.135/video/264/test.264");
        binding.rpPLayer4.startPlay("rtsp://192.168.15.64/m1.avi");
    }
}
