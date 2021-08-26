This is the source code for 6.08 (Intro to EECS via Interconnected Embedded Systems) Final Project at MIT: The Phone ™. The final project asks us to develop a complete  interconnected system on our own. Using a ESP32 DevKit, a ST7735 LCD display, a speaker and DFPlayer Mini MP3 Player, and 3 buttons, our group built a simple phone with functions including messaging, emailing, playing games, checking weather, checking world clock and playing music. While working in a group of 5, I was responsible for implementing the messaging app, email app, user login function and part of the main framework for the phone. More details about the implementation is discussed in WriteUp/FinalReport.

What Does The Phone™ Do
===============================================================================
Brief overview of this phone's functionalities.

Login
-------------------------------------------------------------------------------
The user enters their WiFi network and password to connect to the Internet, then enters their username and password to login into the phone. They have the ability to save their WiFi and user credentials, so next time they can connect and login by just clicking yes.

Home Screen
-------------------------------------------------------------------------------
The home screen displays the icons of all the available apps on the phone. The user can navigate the apps using buttons 1 and 2 (the currently selected app is highlighted). They can enter an app using button 3 and they can navigate back to the home screen with a long press on button 3.

Messaging App
-------------------------------------------------------------------------------
The user can send messages to other users of The Phone™ using their username. Users can also view unread messages or message history with a specific user.

Email App
-------------------------------------------------------------------------------
The user can sign into pre-authorized Gmail accounts (up to 3 accounts). Users can then send new Emails to any Email addresses or to view unread Emails if they have any.

Clock App
-------------------------------------------------------------------------------
The user can view their local time, as well as search up the local time in any city in the world. Additionally, they can favorite up to five cities for easy viewing.

Weather App
-------------------------------------------------------------------------------
The user is able to get the weather information at their current location, or at any location of their own input. Additionally, the app stores the user’s top 5 most looked up locations, so that they can be readily accessed. 

Game App
-------------------------------------------------------------------------------
The user can enter and play a game similar to flappy bird. Using the the buttons, they can jump over obstacles until they crash into one and the game is over. They can then see their score for that round and can go to the leaderboard , which shows the three highest scoring users in the system. The user also has the option to choose between difficulty levels for the game they play.

Music App
-------------------------------------------------------------------------------
Within the music application, the user is able to select one of a maximum of 10 tracks stored in their SD card, and then play it through the MP3 module. 

Notifications
-------------------------------------------------------------------------------
When the user gets new messages or new emails, the phone pushes a banner notification to the top of the screen (regardless of the app the user is currently using). The notification will stay on the screen for 3 seconds unless the user closes it by pressing button 3.

How is the source code organized
===============================================================================
All server code is in python. The main folder contains main.ino and the Phone class (in C++). The Phone class is design to be added as an Arduino library. It contains the phone state machine and each app, which are instances of subclasses of the parent App class. In the main Arduino file an instance of the Phone class is created in the setup function, then in the loop function it calls Phone's loop function. 
