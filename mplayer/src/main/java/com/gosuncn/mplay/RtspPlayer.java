package com.gosuncn.mplay;

import android.Manifest;
import android.content.Context;
import android.opengl.GLSurfaceView;
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

import java.io.File;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class RtspPlayer extends GLSurfaceView {
    private static String TAG = "RtspPlayer";
    private static final int MESSAGE_TYPE_PLAY_CALLBACK=1;
    private static final int MESSAGE_TYPE_PLAY_PREPARE=2;
    public interface OnRtspPlayResultListener {
        /**
         * 播放结果回调
         *
         * @param playerHandle
         * @param code         返回码，1--表示成功
         * @param msg          信息
         */
        void onResult(int playerHandle, int code, String msg);
    }

    OnRtspPlayResultListener listener;

    private Renderer renderer;

    private int mPlayerHandle;

    private Handler handler;//ui线程handler

    private HandlerThread handlerThread;//用于执行耗时操作
    private Handler threadHandler;//子线程handler

    public RtspPlayer(Context context) {
        this(context, null);
    }

    public RtspPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.i(TAG, "----------- RtspPlayer 构造----------");
        setKeepScreenOn(true);
        setEGLContextFactory(new ContextFactory());
        setEGLConfigChooser(new ConfigChooser(5, 6, 5, 0, 0, 0));
        initHandler();
        mPlayerHandle = PlayerJNI.getInstance().createRtspPlayer();
        Log.e(TAG, "mPlayerHandle=" + mPlayerHandle);
        renderer = new Renderer();
        setRenderer(renderer);
    }

    private void initHandler() {
        handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if (msg.what == 1) {
                    if (listener != null) {
                        listener.onResult(mPlayerHandle, msg.arg1, (String) msg.obj);
                    }

                }
            }
        };

        handlerThread=new HandlerThread("RtspPlayer");
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
    }

    /**
     * 设置播放结果回调监听器
     *
     * @param listener
     */
    public void setOnRtspPlayResultListener(OnRtspPlayResultListener listener) {
        this.listener = listener;
    }

    /**
     * 获得播放器的句柄
     *
     * @return
     */
    public int getPlayerHandle() {
        return mPlayerHandle;
    }


    @Override
    public void onPause() {
        super.onPause();
        Log.i(TAG, "----------- GLSurfaceView onPause----------");
        stopPlay();
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(TAG, "----------GLSurfaceView onResume-----------");

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "---------surfaceCreated------------");
        //创建播放器
        if (mPlayerHandle == 0) {
            mPlayerHandle = PlayerJNI.getInstance().createRtspPlayer();
            Log.e(TAG, "new mPlayerHandle=" + mPlayerHandle);
        }
        super.surfaceCreated(holder);

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "---------surfaceDestroyed------------");
        //销毁播放器

        PlayerJNI.getInstance().onSurfaceDestroyed(mPlayerHandle);
        mPlayerHandle = 0;

        super.surfaceDestroyed(holder);

    }


    private static class ContextFactory implements
            EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display,
                                        EGLConfig eglConfig) {
            //Log.i("123456", "creating OpenGL ES 2.0 context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
            EGLContext context = egl.eglCreateContext(display, eglConfig,
                    EGL10.EGL_NO_CONTEXT, attrib_list);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display,
                                   EGLContext context) {
            //Log.i(TAG, "destroy OpenGL ES 2.0 context");
            egl.eglDestroyContext(display, context);
        }
    }

    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    private static class ConfigChooser implements
            EGLConfigChooser {

        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs2 = {EGL10.EGL_RED_SIZE, 4,
                EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL10.EGL_NONE};

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            int[] num_config = new int[1];
            egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException(
                        "No configs match configSpec");
            }

            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs,
                    num_config);

            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                      EGLConfig[] configs) {
            for (EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

                if (d < mDepthSize || s < mStencilSize)
                    continue;

                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                        EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                        EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize && g == mGreenSize && b == mBlueSize
                        && a == mAlphaSize)
                    return config;
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                     EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

    }

    private class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            PlayerJNI.getInstance().onDrawFrame(mPlayerHandle);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.i(TAG, "--------GLSurfaceView--onSurfaceChanged---------\n" + width + " " + height);
            PlayerJNI.getInstance().onSurfaceChanged(mPlayerHandle, width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.i(TAG, "--------GLSurfaceView--onSurfaceCreated---------");
            PlayerJNI.getInstance().onSurfaceCreated(mPlayerHandle);
        }
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
        return PlayerJNI.getInstance().prepare(mPlayerHandle, url);
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
            File dir = new File(savePath.subSequence(0, savePath.lastIndexOf("/")).toString());
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
