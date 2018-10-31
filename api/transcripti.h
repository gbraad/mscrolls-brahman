/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


#pragma once

#include "imptypes.h"
#include "control.h"
#include "transcript.h"
#include "textpage.h"
#include "logged.h"
#include "braschema.h"
#include "growbuf.h"

struct Transcript::Imp : public ImpType<Transcript>
{
    Imp(Transcript* host) : ImpType(host) { _init(); }

    void segmentInfo(char id)
    {
        if (id != _currentSegmentId)
        {
            //LOG3("segment id changed to ", (int)id);
            if (!_segmentText.empty()) finish();
            _currentSegmentId = (int)id;
        }
    }

    void charEmit(char c)
    {
        if (c) _segmentText += c;
        else finish();
    }

    static string imagePathToJSON(const string& imagepath)
    {
        GrowString gs;
        gs.append("{\"" BRA_PIC_NAME "\":\"");
        gs.append(imagepath);
        gs.append("\"}");
        gs.add(0);
        return gs.start();
    }

    string assetResolver(const string& s) const
    {
        // for use in text conversion
        return _host->_control->resolveAsset(s);
    }

    string textHTML() const
    {
        GrowString gs;

        // add stylesheet
        if (_host->_linkColor || _host->_consoleEchoColor)
        {
            TextPage::HTMLStyleBegin(gs);
            if (_host->_linkColor)
                TextPage::HTMLStyleSetLinkColor(gs, _host->_linkColor);
            if (_host->_consoleEchoColor)
                TextPage::HTMLStyleSetClassColor(gs, BRA_CONSOLE_ECHO_STYLE,
                                                 _host->_consoleEchoColor);
            TextPage::HTMLStyleEnd(gs);
        }

        TextFormat tf;

        using std::placeholders::_1;
        tf._resolver = std::bind(&Imp::assetResolver, this, _1);
        
        // add main text
        _page.toStringHTML(tf, gs);
        
        gs.add(0);

        //LOG3("textHTML: ", gs.start());
             
        return gs.start();
    }

    string styleHTML() const
    {
        // used to directly add stylesheet to text
        // eg credits page.
        
        GrowString gs;
        TextPage::styleHTML(gs, _host->_linkColor);
        gs.add(0);
        return gs.start();
    }

    void finish()
    {
        bool changed = false;
            
        // test for special segment types which are UI instructions
        if (_currentSegmentId == BRA_SEGMENT_IMAGE)
        {
            // we are passed the full path or JSON
            string imagepath = trim(_segmentText);

            // unix style path for HTML
            replaceCharsInplace(imagepath, '\\', '/');

            _segmentText.clear();
                    
            if (!imagepath.empty())
            {
                if (imagepath[0] != '{')
                {
                    // non-json image path
                    if (_host->_inlineImages)
                    {
                        _segmentText = "![](";
                        _segmentText += imagepath;
                        _segmentText += ")";
                    }

                    // convert flat image path to JSON
                    imagepath = imagePathToJSON(imagepath);
                }

                LOG3("transcript JSON image ", imagepath);
                    
                // notify controller regardless of inline images
                // NB: parameter is JSON string
                _host->_control->imageChanged(imagepath);
            }
            else
            {
                // clear picture
                imagepath = "{}"; // empty JSON to clear pic
                _host->_control->imageChanged(imagepath);
            }
        }
        else if (_currentSegmentId == BRA_SEGMENT_SOUND)
        {
            //LOG4("sound segment '", _segmentText << "'");
                            
            string js = trim(_segmentText);
            _segmentText.clear();
            if (!js.empty() && js[0] == '{')
            {
                _host->_control->soundChanged(js);
            }
        }
        else if (_currentSegmentId == BRA_SEGMENT_TITLE)
        {
            //LOG3("title segment '", _segmentText << "'");

            _host->_control->titleText(_segmentText);

            _segmentText.clear();

            // no longer indicated through qtransscript
            changed = false; 
        }
        else if (_currentSegmentId == BRA_SEGMENT_JSON)
        {
            //LOG3("JSON segment '", _segmentText << "'");
            _customJSON = _segmentText;
            _segmentText.clear();
            changed = true;
        }
        else if (_currentSegmentId == BRA_SEGMENT_NULL)
        {
            // throw text away and ignore it!
            _segmentText.clear();
        }
            
        if (_currentSegmentId == BRA_SEGMENT_CLEAR)
        {
            _currentSegmentId = 0;
            _page.clear();
                
            changed = true;
        }

        if (!_segmentText.empty())
        {
            if (_currentSegmentId || !_host->_refreshOnNewline)
            {
                // NB: non-ifi is not refreshonnewline
                // so main text goes here.

                // can erase a segment with non-empty whitespace text 
                if (_page.appendSegment(_segmentText, _currentSegmentId))
                    changed = true;
            }
            else
            {
                if (_page.append(_segmentText.c_str())) changed = true;
            }
                
            if (changed)
            {
                //LOG3("transcript, append segment, '", _segmentText << "' (" << _currentSegmentId << ")");
            }
        
            _segmentText.clear();
        }
            
        // emit changed event
        if (changed && _host->_notifier)
            _host->_notifier->changed(_currentSegmentId);
    }

    void _init() 
    {
        _currentSegmentId = 0;

        // set game text to keep 16K of text.
        _page.maxSize(16*1024);
    }

    TextPage            _page;
    int                 _currentSegmentId;
    string              _segmentText;
    string              _customJSON; // for custom UI
};





