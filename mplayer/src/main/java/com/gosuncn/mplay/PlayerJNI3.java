package com.gosuncn.mplay;

/**
 * 服务于RtspPlayer3
 * Created by hwj on 2016/10/18.
 */

public class PlayerJNI3 {
    static {
        System.loadLibrary("streamhandler-lib");
    }
    private PlayerJNI3(){}
    private static PlayerJNI3 instance;
    public static PlayerJNI3 getInstance(){
        if(instance==null){
            instance=new PlayerJNI3();
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
     * 销毁播放器
     * @param handle 通过createRtspPlayer获得
     */
    public native void onSurfaceDestroyed(int handle);

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


    /**
     * 设置surface,适用RtspPlayer2
     * @param handle
     * @param surface
     * @return
     */
    public native int setSurface(int handle, Object surface);


    public native int getRenderCount(int handle);

    public native int getPlayAudioCount(int handle);

    public native int getReceiveVideoCount(int handle);

    public native int getReceiveAudioCount(int handle);

    public native int getDecodeVideoCount(int handle);

    public native int getDecodeAudioCount(int handle);

    public native int getDecodeFailedVideoCount(int handle);

    public native int getDecodeFailedAudioCount(int handle);

}
