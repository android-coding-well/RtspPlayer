package com.gosuncn.mplay;

/**
 * Created by hwj on 2016/10/18.
 */

public class PlayerJNI {
    static {
        System.loadLibrary("native-lib");
    }
    private PlayerJNI(){}
    private static PlayerJNI instance;
    public static PlayerJNI getInstance(){
        if(instance==null){
            instance=new PlayerJNI();
        }
        return instance;
    }

    /**
     * 根据返回码获得错误提示
     * @param code
     * @return
     */
    public native String getMessageByReturnCode(int code);

    /**
     * 销毁播放器
     * @param handle
     */
    public  native void close(int handle);

    /**
     * 开始播放
     * @param handle
     * @return
     */
    public native void startPlay(int handle);

    /**
     * 播放前准备
     * @param handle
     * @param url
     * @return
     */
    public native int prepare(int handle,String url);

    /**
     * 停止播放
     * @param handle
     * @return
     */
    public native void stopPlay(int handle);

    /**
     * 创建播放器，获得handle
     * @return handle
     */
    public native int createRtspPlayer();

    /**
     * 与GLSurfaceView.Renderer接口相对应
     * @param handle 通过createRtspPlayer获得
     */
    public native void onSurfaceCreated(int handle);

    /**
     * 与GLSurfaceView.Renderer接口相对应
     * @param handle 通过createRtspPlayer获得
     * @param width
     * @param height
     */
    public native void onSurfaceChanged( int handle,int width, int height);

    /**
     * 销毁播放器
     * @param handle 通过createRtspPlayer获得
     */
    public native void onSurfaceDestroyed(int handle);

    /**
     *与GLSurfaceView.Renderer接口相对应
     * @param handle 通过createRtspPlayer获得
     */
    public native void onDrawFrame(int handle);

    /**
     * 抓拍（只支持jpg）
     * @param handle 通过createRtspPlayer获得
     * @param path   保存路径，包含文件名
     * @return
     */
    public native int capture(int handle,String path);

    /**
     * 声音开关
     * @param handle
     * @param on
     */
    public native void sound(int handle,boolean on);


    //******************************播放器另一种实现方式需要的接口****************************************

    /**
     * 播放前准备，适用RtspPlayer2
     * @param handle
     * @param url
     * @return
     */
    public native int prepare2(int handle,String url);

    /**
     * 设置surface,适用RtspPlayer2
     * @param handle
     * @param surface
     * @return
     */
    public native int setSurface(int handle, Object surface);

}
