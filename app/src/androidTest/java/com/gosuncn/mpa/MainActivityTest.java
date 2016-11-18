package com.gosuncn.mpa;


import android.support.test.espresso.ViewInteraction;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.gosuncn.mpa.player1.TestPlayer1Activity;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.Espresso.pressBack;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.action.ViewActions.closeSoftKeyboard;
import static android.support.test.espresso.action.ViewActions.replaceText;
import static android.support.test.espresso.action.ViewActions.scrollTo;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withParent;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.allOf;

@LargeTest
@RunWith(AndroidJUnit4.class)
public class MainActivityTest {

    @Rule
    public ActivityTestRule<TestPlayer1Activity> mActivityTestRule = new ActivityTestRule<>(TestPlayer1Activity.class);

    @Test
    public void mainActivityTest() {
        pressBack();

        ViewInteraction appCompatButton = onView(
                allOf(withId(R.id.btn_play), withText("play")));
        appCompatButton.perform(scrollTo(), click());

        ViewInteraction appCompatButton2 = onView(
                allOf(withId(R.id.btn_stop), withText("stop")));
        appCompatButton2.perform(scrollTo(), click());

        ViewInteraction appCompatButton3 = onView(
                withText("play"));
        appCompatButton3.perform(scrollTo(), click());

        ViewInteraction appCompatButton4 = onView(
                allOf(withId(R.id.btn_play), withText("play")));
        appCompatButton4.perform(scrollTo(), click());

        pressBack();

        ViewInteraction appCompatEditText = onView(
                allOf(withId(R.id.et_rtsp_url2), withText("rtsp://1:1@192.168.15.65:554/xms/realplay/44011201/8/35?streams=1"),
                        withParent(withId(R.id.activity_main))));
        appCompatEditText.perform(scrollTo(), replaceText("rtsp://1:1@192.168.15.65:554/xms/realplay/44011201/8/35?streams=1"), closeSoftKeyboard());

        pressBack();

        ViewInteraction appCompatButton5 = onView(
                allOf(withId(R.id.btn_stop), withText("stop")));
        appCompatButton5.perform(scrollTo(), click());

    }

}
