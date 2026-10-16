# Changelog

## v1.3.4
- **Story money no longer resets to $0.** Right after a save loaded, a game script set every character's cash to $0, so each save after that kept $0. The app now ignores that one write while a save loads (#10). Saves made after the bug already hold $0, so money lost before this update doesn't come back.
- **Downloading the game in the app works with more file hosts.** The app asks for the file exactly the way Safari does, in one request. If a host shows a bot check (such as Cloudflare's), the link opens in the app's browser, which passes it and hands the download back. If a site refuses the download because of the ad blocking, the browser turns it off for that site; a shield button does the same by hand.
- **Fewer crashes after long jumps** (character switches, mission skips, respawns): the game could try to draw part of the map it had just unloaded, and crash.
- **Update notice.** When a newer version is out, the app says so once and links to the Releases topic in the Telegram group.
- **Settings are grouped:** Graphics, Controls, Story cheats, Game mode and language, Game files and This device each have their own page.
- **The game pauses when you leave it.** Control Center, a notification, a call, another app or the app's own Settings now open the game's pause menu, and it stays paused until you resume (#11). A story cheat picked from Settings leaves the pause menu by itself.
- **Set money** (Settings › Story cheats): type an amount and the character you're playing gets it. Save the game to keep it.
- **Sprint button** (Settings › Controls, off by default): tap it while moving to sprint until you let go of the stick; hold it to open a parachute.
- **Airplane right stick** (Settings › Controls, off by default): a second stick for planes; drag up or down to set the throttle (it stays where you leave it), left or right for rudder. Helicopters keep their buttons. Thanks to GitHub user @m1337v (#12).
- **Game files that fail to read are named in the log**, and a read the system interrupted is retried instead of failing. Thanks to GitHub user @m1337v (#13).
- **Less texture memory on iPhones.** The most common texture type is now stored in a format the iPhone's GPU reads directly, at half the memory of before.
- **Audio uses a little less CPU:** the game's music and speech are decoded natively.
- **Extra memory is asked for by default** in the Mac script, the GitHub workflow and the builder site; use `--free` (or untick the box) for a free Apple ID.
- **Builder site:** coming back after a new version is out now offers to build it, instead of downloading your previous build again (#14).

Thanks to @m1337v for the airplane right stick and the file read fix (#12, #13), and to @aliiscoding2122, @k49kg9xmsd-prog and @phakamaec for the issues behind the money fix, the pause and the builder fix (#10, #11, #14).

## v1.3.3
- **Runs on iOS 16.** The app now installs on iOS 16 as well as iOS 17 and later.
- **No more crash at the end of Marriage Counseling** (the Vinewood Hills fences). The engine wrote the list of where a model's pieces are in memory one entry past the space the model reserved for it, which broke the fence model. That same overflow hit other models too, so it may fix other rare glitches as well.
- **The game's intro plays when you press Play.** The original Rockstar intro runs while the game loads behind it, instead of the blue loading log. Tap to skip.
- **Runs cooler and stutters less.**
  - The game's audio processing uses about a third less CPU (the whole app about 12% less, measured on an iPhone 13 Pro Max).
  - The game no longer plans its textures as if every phone had 4 GB of video memory; the texture budget now fits your phone's memory.
  - New switch, Settings › Graphics › Steady frame rate when hot: lowers the frame rate while the iPhone is hot for fewer stutters, and raises it again once it cools down. Off by default.
- **No more app-wide lag and "stuck" buttons late in a session.** The game could print millions of log lines, and each one went through the app's main thread, so menus lagged and buttons acted as if held. Thanks to the player who traced it.
- **No more lock-ups after long sessions on phones with lots of memory.** The game's memory was capped at 4 GB even where the phone allowed more; phones with a limit of about 7.5 GB or more now give it up to 6 GB, and the app backs off loading instead of retrying when the game's memory is full.
- **Memory cap**, Settings › Graphics: limit how much memory the game may use (Automatic by default). Lower it if the game closes on its own.
- **Story cheats in Settings.** While playing, Settings lists the game's own cheats (invincibility, health and armor, weapons, wanted level, Buzzard, Comet and more); tap one and the app enters it for you. Thanks to GitHub user @ITSUPERVISOR (#9).
- **Steering buttons**, Settings › Touch controls: Left and Right buttons instead of the steering stick in cars, bikes and boats.
- **Phone keypad with touch controls.** A new button on the phone controls (the controller's X) opens the keypad for a custom number and the phone's other options.
- **Taxi rides work with touch controls** (Story mode; Sandbox has no taxi service). As a passenger, Left/Right change the destination, Select picks it, and there are Hurry up, Stop and Exit buttons (#6).
- **The aircraft weapon button switches weapons** (cannon and missiles) instead of changing the radio station.
- **Game files without the GTA VI test map work.** Some copies of the game leave out the test level; the file check no longer calls them incomplete, and the GTA VI test map button greys out instead.
- **Settings during play only shows what works during play.** Game mode, Sandbox map, game files, graphics presets and frame rate apply at the next start, so they're hidden while you play.
- **Telegram links** on the start screen and in Settings. The Telegram group is the official place for updates and help.
- **One IPA for everyone.** It works with SideStore, AltStore and Sideloadly, and gets the extra memory where your account allows it.
- **Clearer logs.** Hundreds of engine messages that printed as empty "Error:" lines now show their text, repeats are collapsed, sent logs keep the start of the run as well as the end, and logs report heat, CPU use and stutters every 10 seconds.

Thanks to everyone who sent logs with exact failures and their own analysis: those reports found the Story mode bugs fixed here (the Marriage Counseling crash, the lock-ups after long sessions, the lag with stuck buttons).

## v1.3.2
- **Fewer missing and blurry textures.** The app used much more memory for textures than the game planned for, so the game threw detailed textures away to stay under the limit.
  - iPads and Macs with Apple chips keep textures compressed: 2 to 8 times less texture memory.
  - iPhones store the most common texture type at half the size.
  - A spare copy of every texture is no longer kept in memory.
  - The game stops loading new detail a little before the memory limit instead of loading and dropping the same textures over and over.
- **13 languages** for the app's menus (English, French, Italian, German, Spanish, Latin American Spanish, Japanese, Korean, Polish, Brazilian Portuguese, Russian, Simplified and Traditional Chinese). The game itself switches language the next time it starts. Thanks to GitHub user @k49kg9xmsd-prog (#4).
- **Swipe to look** (Settings › Controls › Camera): swipe anywhere to move the camera. It moves the game's mouse, so it is smooth in every direction. Sensitivity goes up to 15x, 5x by default. Thanks to GitHub user @k49kg9xmsd-prog (#4).
- **Pause map:** drag on the left side of the screen to move the cursor and set a waypoint anywhere, not only on places in the legend.
- **Shop menus** (Ammu-Nation, clothes and others) show the D-pad so you can move through them.
- **New vehicle buttons:**
  - Tow trucks: tow hook, and Raise and Lower for the tow arm.
  - Convertibles: roof up and down.
- **Switching aircraft weapons** no longer opens the radio wheel.
- **GTA VI test map** gets the phone, cheats and free roam, and puts you back on land if you fall off the map.
- **Fixed a crash on two fences in the Vinewood Hills** ("Invalid fixup").
- Fixed a crash when pedestrians spawn at a crowded spot.
- The Replay editor no longer says there's no space left.
- Props no longer flicker in the first time you see them.
- Thanks to tomergamertv on Telegram for the last three.
- **New start screen and Settings background.**
- **FPS counter switch** in Settings › Graphics.
- **Settings shows whether extra memory is on** for your install, next to the memory limit.
- Logs say more: the app version, where your game files are, free storage and memory, and which engine function a crash happened in.

## v1.3.1
- **Los Santos Customs works.** Driving in no longer leaves you stuck inside with no menu: the shop waited for a texture the game files don't include. A few labels in the shop may be blank.
- **Interact shows in vehicles too** when the game asks for it ("Press D-pad right to ...").
- **The app version shows** under the title on the start screen, in Settings and in the log. Please mention it when you report a problem.

## v1.3
- **No more freezes when a TV comes on.** The game waited forever for TV and cutscene videos the game files don't include (Franklin's aunt's house, Michael's house, the Complications intro). Those videos are now skipped.
- **The minimap shows the map** (it was blank in every earlier version, and in the browser version).
- **New start screen** with new artwork, a new app icon, and a matching Settings look.
- **Game files screen:**
  - The app finds your game anywhere in its folder, however deep, or in any folder you choose (even On My iPhone itself).
  - **Every game file is checked.** If something is missing or damaged, it names the file and the likely cause (usually unpacking stopped early for lack of storage). **Play anyway** if only a few files are off.
  - **Unpack in the app:** paste a direct link to your zip or rar, or pick the archive in Files. It unpacks as it downloads, so you need about 21 GB free, not double. Dropped connections resume.
  - **Download pages** ("tap Download to start") open in a built-in browser with ad and pop-up blocking; the app takes the download over by itself.
  - Downloads keep going while you look around the start screen, Settings or the controls layout.
- **Sandbox map choice:** GTA V, or the GTA VI test map that comes with the game files.
- **Controls:**
  - New buttons that show only when they apply: special ability (with its charge), detonate (sticky bombs placed), interact (when the game asks), zoom while aiming, and weapons inside a weapon wheel slot.
  - Radio: tap for the next station, hold and point at a station on the wheel. Controllers pick with the right stick.
  - Cars: tilt in the air with up/down on the stick.
  - Aircraft: throttle and rudder buttons instead of the hidden throttle lever.
  - Quick taps are no longer missed.
  - All new buttons can be moved in the controls layout editor, which has an Aircraft tab now.
- **Keyboard and mouse** (PC controls) when connected; touch controls hide when both are connected.
- **Apple Game Mode** turns on while you play.
- Changing graphics in the game's own pause menu can no longer stop the game from starting.
- Crash logs survive reopening the app several times before sending.

## v1.2.2
- New app icon.
- **Edit controls layout** (start screen or Settings): move, resize or hide every touch button, on foot and in vehicles.
- **Accept / Back buttons** on screens like Mission Failed, so you can retry without a controller.
- **Switch character** button in Story mode (hold, drag toward a character, release).
- The **phone stays up** when you get into a vehicle; Back closes it.
- Touch controls **hide while a game controller is connected** (setting, on by default).
- **Frame rate 30, 60 or Unlocked** (up to 120 Hz on ProMotion iPhones).
- Default camera look speed doubled.
- **Send log to the developer** in one tap; the log records memory and file-loading health every 10 seconds.
- README: "Get the most performance" guide (SideStore + GetMoreRam).

## v1.2.1
- Move your game folder into **Files › On My iPhone › GTA V Port** and the app finds it by itself.
- Picking the wrong folder now explains what to pick instead.
- Any folder above the game folder can be picked; folder picks are logged.

## v1.2
- **Build from your iPhone** with [5★ Port Builder](https://fivestarport.vercel.app), no computer needed.
- **Play from a folder on your iPhone**: no server, nothing streams while playing.
- New **Max** preset, every preset selectable, separate resolution setting that changes while playing.
- After a launch that runs out of memory, the next start explains it and lowers the settings.
- **Share log** in Settings.

## v1.1
- **Native Metal renderer**: about 2.2 to 2.6× the frame rate of v1.0 and about 700 MB less memory.
- **Story mode or Sandbox**.
- **Download all game files** to the phone (streaming route).

## v1.0
- First release: build the iPhone app from your own game files with GitHub Actions or a Mac, streaming the files from your computer over Tailscale.
