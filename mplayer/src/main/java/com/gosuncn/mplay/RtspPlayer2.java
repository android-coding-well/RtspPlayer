package com.gosuncn.mplay;

import android.Manifest;
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresPermission;
import android.support.annotation.WorkerThread;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

/**
 * Created by hwj on 2016/11/17.
 */

public class RtspPlayer2 extends SurfaceView implements SurfaceHolder.Callback{
    private static final String TAG = "RtspPlayer2";
    private static final int MESSAGE_TYPE_PLAY_CALLBACK=1;
    private static final int MESSAGE_TYPE_PLAY_PREPARE=2;
    private int mPlayerHandle;
    private Handler handler;//ui线程handler

    private HandlerThread handlerThread;//用于执行耗时操作
    private Handler threadHandler;//子线程handler

    public RtspPlayer2(Context context) {
        this(context,null,0);
    }

    public RtspPlayer2(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public RtspPlayer2(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "---------surfaceCreated------------");
        //创建播放器
        if (mPlayerHandle == 0) {
            mPlayerHandle = PlayerJNI.getInstance().createRtspPlayer();
            Log.e(TAG, "new mPlayerHandle=" + mPlayerHandle);
        }
        //
        PlayerJNI.getInstance().setSurface(mPlayerHandle,holder.getSurface());
    }


    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }


    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "---------surfaceDestroyed------------");
        //销毁播放器
        PlayerJNI.getInstance().onSurfaceDestroyed(mPlayerHandle);
        mPlayerHandle = 0;
    }

    public interface OnRtspPlayResultListener {
        /**
         * 播放结果回调
         * @param playerHandle
         * @param code          返回码，1--表示成功
         * @param msg           信息
         */
        void onResult(int playerHandle, int code, String msg);
    }

    RtspPlayer.OnRtspPlayResultListener listener;
    /**
     * 设置播放结果回调监听器
     * @param listener
     */
    public void setOnRtspPlayResultListener(RtspPlayer.OnRtspPlayResultListener listener) {
        this.listener = listener;
    }
    private void init() {
        handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if (msg.what == MESSAGE_TYPE_PLAY_CALLBACK) {
                    if (listener != null) {
                        listener.onResult(mPlayerHandle, msg.arg1, (String) msg.obj);
                    }

                }
            }
        };
        handlerThread=new HandlerThread("RtspPlayer2");
        handlerThread.start();

        threadHandler=new Handler(handlerThread.getLooper()){
            @Override
            public void handleMessage(Message msg) {
               switch(msg.what){
                   case MESSAGE_TYPE_PLAY_PREPARE://prepare
                       int ret = PlayerJNI.getInstance().prepare2(mPlayerHandle, (String)msg.obj);
                       handler.sendMessage(handler.obtainMessage(MESSAGE_TYPE_PLAY_CALLBACK, ret, 0, PlayerJNI.getInstance().getMessageByReturnCode(ret)));
                       if (ret == 1) {
                           PlayerJNI.getInstance().startPlay(mPlayerHandle);
                       }
                   break;
               }
            }
        };

        getHolder().addCallback(this);
        mPlayerHandle=PlayerJNI.getInstance().createRtspPlayer();
    }


    /**
     * 停止播放(默认情况下会在播放器销毁前调用，但是依然希望创建者在适合时机主动调用此方法)
     */
    public void stopPlay() {
        if (mPlayerHandle != 0) {
            PlayerJNI.getInstance().stopPlay(mPlayerHandle);
        }
    }

    /**
     * 播放前请先调用prepare确认是否能播放
     */
    @WorkerThread
    public void play() {
        if (mPlayerHandle == 0) {
            return;
        }
        PlayerJNI.getInstance().startPlay(mPlayerHandle);
    }
    /**
     * 播放前准备，由于此方法是耗时操作，必须在子线程执行
     *
     * @param url
     * @return
     */
    @WorkerThread
    public int prepare(String url) {
        if (mPlayerHandle == 0) {
            return -1;
        }
        return PlayerJNI.getInstance().prepare2(mPlayerHandle, url);
    }

    /**
     *  播放
     * @param url
     */
    public void startPlay(final String url) {
        if (mPlayerHandle == 0) {
            handler.sendMessage(handler.obtainMessage(MESSAGE_TYPE_PLAY_CALLBACK, -1, 0, "未创建播放器"));
            return;
        }
        threadHandler.sendMessage(threadHandler.obtainMessage(MESSAGE_TYPE_PLAY_PREPARE,url));

    }

    /**
     * 抓拍
     *
     * @param savePath 保存路径
     * @return
     */
    @RequiresPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)
    public int capture(@NonNull String savePath) {
        if (!TextUtils.isEmpty(savePath)) {
            File dir = new File(savePath.subSequence(0,savePath.lastIndexOf("/")).toString());
            if (!dir.exists()) {
                dir.mkdirs();
            }
            return PlayerJNI.getInstance().capture(mPlayerHandle, savePath);
        }
        return -1;
    }

    /**
     * 抓拍
     *
     * @param direction 保存目录
     * @param filename  文件名上
     * @return
     */
    @RequiresPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)
    public int capture(@NonNull String direction, @NonNull String filename) {
        if (!TextUtils.isEmpty(direction) && !TextUtils.isEmpty(filename)) {
            File dir = new File(direction);
            if (!dir.exists()) {
                dir.mkdirs();
            }
            return PlayerJNI.getInstance().capture(mPlayerHandle, direction + "/" + filename);
        }
        return -1;
    }

    /**
     * 打开声音
     */
    public void openSound() {
        PlayerJNI.getInstance().sound(mPlayerHandle, true);
    }

    /**
     * 关闭声音
     */
    public void closeSound() {
        PlayerJNI.getInstance().sound(mPlayerHandle, false);
    }

    /**
     * 销毁播放器（默认情况下会自动调用，但这里依然开放出来）
     */
    public void close() {
        PlayerJNI.getInstance().close(mPlayerHandle);
    }

}
