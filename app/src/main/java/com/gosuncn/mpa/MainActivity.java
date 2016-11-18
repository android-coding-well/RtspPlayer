package com.gosuncn.mpa;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.gosuncn.mpa.player1.TestPlayer1Activity;
import com.gosuncn.mpa.player2.TestPlayer2Activity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onPlayer1Click(View view) {
        Intent intent = new Intent(this, TestPlayer1Activity.class);
        startActivity(intent);
    }
    public void onPlayer2Click(View view) {
        Intent intent = new Intent(this, TestPlayer2Activity.class);
        startActivity(intent);
    }
}
