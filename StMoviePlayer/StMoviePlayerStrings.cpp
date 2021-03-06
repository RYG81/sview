/**
 * Copyright © 2013-2014 Kirill Gavrilov <kirill@sview.ru>
 *
 * StMoviePlayer program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StMoviePlayer program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "StMoviePlayerStrings.h"

#include <StStrings/StLangMap.h>

namespace StMoviePlayerStrings {

void loadDefaults(StLangMap& theStrings) {
    theStrings(BUTTON_CLOSE,
               "Close");
    theStrings(BUTTON_CANCEL,
               "Cancel");
    theStrings(BUTTON_RESET,
               "Reset");
    theStrings(BUTTON_SAVE_METADATA,
               "Save");
    theStrings(BUTTON_DELETE,
               "Delete");
    theStrings(MENU_MEDIA,
               "Media");
    theStrings(MENU_VIEW,
               "View");
    theStrings(MENU_AUDIO,
               "Audio");
    theStrings(MENU_AUDIO_NONE,
               "None");
    theStrings(MENU_AUDIO_DELAY,
               "Audio/Video delay");
    theStrings(MENU_AUDIO_ATTACH,
               "Attach from file");
    theStrings(DIALOG_AUDIO_DELAY_TITLE,
               "Audio/Video syncronization");
    theStrings(DIALOG_AUDIO_DELAY_DESC,
               "Enter positive value if audio appears earlier than video and negative otherwise.");
    theStrings(DIALOG_AUDIO_DELAY_LABEL,
               "Audio delay:");
    theStrings(DIALOG_AUDIO_DELAY_UNITS,
               "seconds");
    theStrings(MENU_SUBTITLES,
               "Subtitles");
    theStrings(MENU_SUBTITLES_NONE,
               "None");
    theStrings(MENU_SUBTITLES_ATTACH,
               "Attach from file");
    theStrings(MENU_SUBTITLES_SIZE,
               "Font Size");
    theStrings(MENU_SUBTITLES_PARALLAX,
               "Parallax");
    theStrings(MENU_SUBTITLES_PARSER,
               "Parser");
    theStrings(MENU_SUBTITLES_PLAIN_TEXT,
               "Plain text");
    theStrings(MENU_SUBTITLES_LITE_HTML,
               "Lite HTML");
    theStrings(MENU_HELP,
               "Help");
    theStrings(MENU_MEDIA_OPEN_MOVIE,
               "Open Movie...");
    theStrings(MENU_MEDIA_SAVE_SNAPSHOT_AS,
               "Save Snapshot As...");
    theStrings(MENU_MEDIA_SRC_FORMAT,
               "Source stereo format");
    theStrings(MENU_MEDIA_AL_DEVICE,
               "Audio Device"),
    theStrings(MENU_MEDIA_GPU_DECODING,
               "Video decoding on GPU");
    theStrings(MENU_MEDIA_SHUFFLE,
               "Shuffle");
    theStrings(MENU_MEDIA_RECENT,
               "Recent files");
    theStrings(MENU_MEDIA_WEBUI,
               "Web UI");
    theStrings(MENU_MEDIA_WEBUI_OFF,
               "Turn Off");
    theStrings(MENU_MEDIA_WEBUI_ONCE,
               "Launch once");
    theStrings(MENU_MEDIA_WEBUI_ON,
               "Launch every time");
    theStrings(MENU_MEDIA_WEBUI_SHOW_ERRORS,
               "Show errors");
    theStrings(WEBUI_ERROR_PORT_BUSY,
               "Web UI can not be started on {0} port!");
    theStrings(MENU_MEDIA_QUIT,
               "Quit");
    theStrings(MENU_MEDIA_FILE_INFO,
               "File info");
    theStrings(MENU_MEDIA_OPEN_MOVIE_1,
               "From One file");
    theStrings(MENU_MEDIA_OPEN_MOVIE_2,
               "Left+Right files");
    theStrings(MENU_SRC_FORMAT_AUTO,
               "Autodetection");
    theStrings(MENU_SRC_FORMAT_MONO,
               "Mono");
    theStrings(MENU_SRC_FORMAT_CROSS_EYED,
               "Cross-eyed");
    theStrings(MENU_SRC_FORMAT_PARALLEL,
               "Parallel Pair");
    theStrings(MENU_SRC_FORMAT_OVERUNDER_RL,
               "Over/Under (R/L)");
    theStrings(MENU_SRC_FORMAT_OVERUNDER_LR,
               "Over/Under (L/R)");
    theStrings(MENU_SRC_FORMAT_INTERLACED,
               "Interlaced");
    theStrings(MENU_SRC_FORMAT_ANA_RC,
               "Anaglyph Red/Cyan");
    theStrings(MENU_SRC_FORMAT_ANA_RB,
               "Anaglyph Green/Red+Blue");
    theStrings(MENU_SRC_FORMAT_ANA_YB,
               "Anaglyph Yellow/Blue");
    theStrings(MENU_SRC_FORMAT_PAGEFLIP,
               "Frame-sequential");
    theStrings(MENU_SRC_FORMAT_TILED_4X,
               "Tiled 4X");
    theStrings(MENU_SRC_FORMAT_SEPARATE,
               "2 streams");
    theStrings(MENU_MEDIA_RECENT_CLEAR,
               "Clear history");
    theStrings(MENU_VIEW_DISPLAY_MODE,
               "Stereo Output");
    theStrings(MENU_VIEW_FULLSCREEN,
               "Fullscreen");
    theStrings(MENU_VIEW_RESET,
               "Reset");
    theStrings(MENU_VIEW_SWAP_LR,
               "Swap Left/Right");
    theStrings(MENU_VIEW_DISPLAY_RATIO,
               "Display Ratio");
    theStrings(MENU_VIEW_TEXFILTER,
               "Smooth Filter");
    theStrings(MENU_VIEW_IMAGE_ADJUST,
               "Image Adjust");
    theStrings(MENU_VIEW_ADJUST_RESET,
               "Reset to defaults");
    theStrings(MENU_VIEW_ADJUST_BRIGHTNESS,
               "Brightness");
    theStrings(MENU_VIEW_ADJUST_SATURATION,
               "Saturation");
    theStrings(MENU_VIEW_ADJUST_GAMMA,
               "Gamma");
    theStrings(MENU_VIEW_SURFACE,
               "Surface");
    theStrings(MENU_VIEW_SURFACE_PLANE,
               "Plane");
    theStrings(MENU_VIEW_SURFACE_SPHERE,
               "Sphere");
    theStrings(MENU_VIEW_SURFACE_CYLINDER,
               "Cylinder");
    theStrings(MENU_VIEW_DISPLAY_MODE_STEREO,
               "Stereo");
    theStrings(MENU_VIEW_DISPLAY_MODE_LEFT,
               "Left view");
    theStrings(MENU_VIEW_DISPLAY_MODE_RIGHT,
               "Right view");
    theStrings(MENU_VIEW_DISPLAY_MODE_PARALLEL,
               "Parallel pair");
    theStrings(MENU_VIEW_DISPLAY_MODE_CROSSYED,
               "Cross-eyed pair");
    theStrings(MENU_VIEW_DISPLAY_RATIO_SRC,
               "Source");
    theStrings(MENU_VIEW_KEEP_ON_RESTART,
               "Keep on restart");
    theStrings(MENU_VIEW_TEXFILTER_NEAREST,
               "Nearest");
    theStrings(MENU_VIEW_TEXFILTER_LINEAR,
               "Linear");
    theStrings(MENU_VIEW_TEXFILTER_BLEND,
               "Blend Deinterlace");
    theStrings(MENU_CHANGE_DEVICE,
               "Change Device");
    theStrings(MENU_ABOUT_RENDERER,
               "About Plugin...");
    theStrings(MENU_FPS,
               "FPS Control");
    theStrings(MENU_FPS_VSYNC,
               "VSync");
    theStrings(MENU_FPS_METER,
               "Show FPS");
    theStrings(MENU_FPS_BOUND,
               "Reduce CPU usage");

    theStrings(ABOUT_DPLUGIN_NAME,
               "sView - Movie Player");
    theStrings(ABOUT_VERSION,
               "version");
    theStrings(ABOUT_DESCRIPTION,
               "Movie player allows you to play stereoscopic video.\n"
               "(C) 2007-2015 Kirill Gavrilov <kirill@sview.ru>\n"
               "Official site: www.sview.ru\n"
               "\n"
               "This program is distributed under GPL3.0");
    theStrings(ABOUT_SYSTEM,
               "System Info");
    theStrings(MENU_HELP_ABOUT,
               "About...");
    theStrings(MENU_HELP_USERTIPS,
               "User Tips");
    theStrings(MENU_HELP_LICENSE,
               "License text");
    theStrings(MENU_HELP_SYSINFO,
               "System Info");
    theStrings(MENU_HELP_SCALE,
               "Scale Interface");
    theStrings(MENU_HELP_SCALE_SMALL,
               "Small");
    theStrings(MENU_HELP_SCALE_NORMAL,
               "Normal");
    theStrings(MENU_HELP_SCALE_BIG,
               "Big");
    theStrings(MENU_HELP_SCALE_HIDPI2X,
               "Force HiDPI 2X");
    theStrings(MENU_HELP_EXPERIMENTAL,
               "Experimental features");
    theStrings(MENU_HELP_BLOCKSLP,
               "Block sleeping");
    theStrings(MENU_HELP_UPDATES,
               "Check for updates");
    theStrings(MENU_HELP_LANGS,
               "Language");
    theStrings(MENU_HELP_BLOCKSLP_NEVER,
               "Never");
    theStrings(MENU_HELP_BLOCKSLP_PLAYBACK,
               "During Playback");
    theStrings(MENU_HELP_BLOCKSLP_FULLSCR,
               "When in fullscreen");
    theStrings(MENU_HELP_BLOCKSLP_ALWAYS,
               "Always");
    theStrings(MENU_HELP_UPDATES_NOW,
               "Now");
    theStrings(MENU_HELP_UPDATES_DAY,
               "Each day");
    theStrings(MENU_HELP_UPDATES_WEEK,
               "Each week");
    theStrings(MENU_HELP_UPDATES_YEAR,
               "Each year");
    theStrings(MENU_HELP_UPDATES_NEVER,
               "Never");
    theStrings(FILE_VIDEO_OPEN,
               "Open another movie");
    theStrings(BTN_SRC_FORMAT,
               "Source format:");
    theStrings(VIDEO_PLAYPAUSE,
               "Play/Pause");
    theStrings(VIDEO_LIST_PREV,
               "Play Previous File");
    theStrings(VIDEO_LIST_NEXT,
               "Play Next File");
    theStrings(VIDEO_LIST,
               "Show/Hide playlist");
    theStrings(FULLSCREEN,
               "Switch\nfullscreen/windowed");
    theStrings(UPDATES_NOTIFY,
               "A new version of sView is available on the official site www.sview.ru.\n"
               "Please update your program.");
    theStrings(DIALOG_OPEN_LEFT,
               "Choose LEFT video file to open");
    theStrings(DIALOG_OPEN_FILE,
               "Choose the video file to open");
    theStrings(DIALOG_OPEN_RIGHT,
               "Choose RIGHT video file to open");
    theStrings(DIALOG_FILE_INFO,
               "File Info");
    theStrings(DIALOG_FILE_NOINFO,
               "Information is unavailable");
    theStrings(DIALOG_DELETE_FILE_TITLE,
               "File deletion");
    theStrings(DIALOG_DELETE_FILE_QUESTION,
               "Do you really want to completely remove this file?");
    theStrings(DIALOG_FILE_DECODERS,
               "Active decoders:");
    theStrings(DIALOG_NOTHING_TO_SAVE,
               "Nothing to save!");
    theStrings(DIALOG_NO_SNAPSHOT,
               "Snapshot not available!");
    theStrings(DIALOG_SAVE_SNAPSHOT,
               "Choose location to save snapshot");

    theStrings(INFO_LEFT,
               "[left]");
    theStrings(INFO_RIGHT,
               "[right]");
    theStrings(INFO_FILE_NAME,
               "File name(s)");
    theStrings(INFO_DIMENSIONS,
               "Video dimensions");
    theStrings(INFO_LOAD_TIME,
               "Load time");
    theStrings(INFO_TIME_MSEC,
               "msec");
    theStrings(INFO_PIXEL_RATIO,
               "Pixel ratio");
    theStrings(INFO_PIXEL_FORMAT,
               "Pixel format");
    theStrings(INFO_NO_SRCFORMAT,
               "(does not stored in metadata)");
    theStrings(INFO_WRONG_SRCFORMAT,
               "(does not match metadata)");
    theStrings(INFO_DURATION,
               "Duration");
    theStrings(INFO_NO_SRCFORMAT_EX,
               "(does not stored in metadata\nbut detected from file name)");

    theStrings(METADATA_TITLE,
               "Title");
    theStrings(METADATA_COMPOSER,
               "Composer");
    theStrings(METADATA_ARTIST,
               "Artist");
    theStrings(METADATA_ALBUM_ARTIST,
               "Album artist");
    theStrings(METADATA_ALBUM,
               "Album");
    theStrings(METADATA_DISC,
               "Disc");
    theStrings(METADATA_DISC_TOTAL,
               "Nb. of discs");
    theStrings(METADATA_GENRE,
               "Genre");
    theStrings(METADATA_COMMENT,
               "Comment");
    theStrings(METADATA_NOTES,
               "Notes");
    theStrings(METADATA_DESCRIPTION,
               "Description");
    theStrings(METADATA_PUBLISHER,
               "Publisher");
    theStrings(METADATA_COPYRIGHT,
               "Copyright");
    theStrings(METADATA_ENCODER,
               "Encoder");
    theStrings(METADATA_ENGINEER,
               "Engineer");
    theStrings(METADATA_SOURCE,
               "Source");
    theStrings(METADATA_CREATION_TIME,
               "Creation time");
    theStrings(METADATA_DATE,
               "Date");
    theStrings(METADATA_YEAR,
               "Year");
    theStrings(METADATA_LANGUAGE,
               "Language");
    theStrings(METADATA_TRACK,
               "Track");
    theStrings(METADATA_TRACK_TOTAL,
               "Nb. of tracks");
    theStrings(METADATA_TRACK_GAIN,
               "Track gain");
    theStrings(METADATA_TRACK_PEAK,
               "Track peak");
    theStrings(METADATA_ALBUM_GAIN,
               "Album gain");
    theStrings(METADATA_ALBUM_PEAK,
               "Album peak");

    // define metadata keys, should be lower cased
    theStrings.addAlias("title",         METADATA_TITLE);
    theStrings.addAlias("composer",      METADATA_COMPOSER);
    theStrings.addAlias("artist",        METADATA_ARTIST);
    theStrings.addAlias("album_artist",  METADATA_ALBUM_ARTIST);
    theStrings.addAlias("album artist",  METADATA_ALBUM_ARTIST);
    theStrings.addAlias("album",         METADATA_ALBUM);
    theStrings.addAlias("disc",          METADATA_DISC);
    theStrings.addAlias("disctotal",     METADATA_DISC_TOTAL);
    theStrings.addAlias("totaldiscs",    METADATA_DISC_TOTAL);
    theStrings.addAlias("genre",         METADATA_GENRE);
    theStrings.addAlias("comment",       METADATA_COMMENT);
    theStrings.addAlias("notes",         METADATA_NOTES);
    theStrings.addAlias("description",   METADATA_DESCRIPTION);
    theStrings.addAlias("publisher",     METADATA_PUBLISHER);
    theStrings.addAlias("copyright",     METADATA_COPYRIGHT);
    theStrings.addAlias("encoder",       METADATA_ENCODER);
    theStrings.addAlias("encoded_by",    METADATA_ENCODER);
    theStrings.addAlias("engineer",      METADATA_ENGINEER);
    theStrings.addAlias("source",        METADATA_SOURCE);
    theStrings.addAlias("creation_time", METADATA_CREATION_TIME);
    theStrings.addAlias("date",          METADATA_DATE);
    theStrings.addAlias("year",          METADATA_YEAR);
    theStrings.addAlias("language",      METADATA_LANGUAGE);
    theStrings.addAlias("track",         METADATA_TRACK);
    theStrings.addAlias("tracktotal",    METADATA_TRACK_TOTAL);
    theStrings.addAlias("totaltracks",   METADATA_TRACK_TOTAL);
    theStrings.addAlias("replaygain_track_gain", METADATA_TRACK_GAIN);
    theStrings.addAlias("replaygain_track_peak", METADATA_TRACK_PEAK);
    theStrings.addAlias("replaygain_album_gain", METADATA_ALBUM_GAIN);
    theStrings.addAlias("replaygain_album_peak", METADATA_ALBUM_PEAK);
    //theStrings.addAlias("album dynamic range",   METADATA_ALBUM_DYNAMIC_RANGE);
    //theStrings.addAlias("dynamic range",         METADATA_DYNAMIC_RANGE);
}

};
