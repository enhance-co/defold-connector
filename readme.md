# Enhance Connector for Defold

Setup
-----

Include the provided extension folder into your Defold project, or add this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

    https://github.com/enhance-co/defold-connector/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/enhance-co/defold-connector/releases).

Interstitial Ads
----------------

Interstitial Ads are short static or video ads, usually shown between levels or when game is over.

1) Check whether any ad is available:

       is_ready = enhance.isInterstitialReady("default")

2) Show the ad:
    
       enhance.showInterstitial("default")

Note:
Interstitial ads will automatically interrupt the flow of your application until the user completes interaction with the ad.


Rewarded Ads
------------

Rewarded Ads are usually video ads which pay high CPM for the users who view them in return for an in-game reward.

1) Check whether any ad is available:

       isReady = enhance_extension.isRewardedAdReady("neutral")

2) Show the ad:

       enhance.showRewardedAd("placement", on_reward_granted, on_reward_declined, on_reward_unavailable)

Note:
Rewarded ads will automatically interrupt the flow of your application until the user completes interaction with the ad.


Banner Ads
----------

Banner Ads are small sized ads displayed in the top or bottom of the app.

1) Check whether any ad is available:

       isReady = enhance.isBannerAdReady(optionalPlacement)

2) Display the ad:

       enhance.showBannerAd("placement", "position");
       enhance.showBannerAd("position");

    Available positions:

       "top"
       "bottom"

3) Hide the ad:

       enhance.hideBannerAd()

Note:
You can also integrate Fixed Banner Ads through Enhance ZeroCode with no coding at all.


Offer Walls
-----------

Offer Walls show full screen of real world offers(e.g. surveys), usually with reward offered in return for a completion.

1) Check whether any offer wall is available:

       isReady = enhance.isOfferwallReady(optionalPlacement)

2) Display the offer wall:

       enhance.showOfferwall(optionalPlacement)

Note:
Offer walls will automatically interrupt the flow of your application until the user completes interaction with the offer wall.


Special Offers
--------------

Special offers are real world offers(e.g. surveys). They are available through Enhance ZeroCode, but now you can also request them from code.

1) Check whether any special offer is available:

       isReady = enhance.isSpecialOfferReady(optionalPlacement)

2) Display the special offer:

       enhance.showSpecialOffer(optionalPlacement)

Note:
Special Offers will automatically interrupt the flow of your application until the user completes interaction with the special offer.

Analytics
---------

Enhance can hook into various analytics networks. By default it will track sessions, users and retention.

1) Send a simple analytics event:

       enhance.logCustomEvent("event_name")

2) Send a detailed analytics event:

       enhance.logCustomEvent("game_over", "level", "10")


Local Notifications
-------------------

Local Notifications are reminders displayed on your mobile device's screen after the app becomes inactive.

1) Ask for local notifications permission:

       enhance.requestLocalNotificationPermission(grantedCallback, refusedCallback)

2) Enable a new local notification:

       enhance.enableLocalNotification("Enhance", "Local Notification!", 5)

3) Disable the local notification:

       enhance.disableLocalNotification()


Demo Project
--------------

For a full implementation example, please see the demo project which can be found in the 'demo_project' directory within the distribution package.

Please remember that none of these features will work properly before Enhancing your application on our website.
