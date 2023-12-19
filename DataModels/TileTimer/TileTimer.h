#ifndef TILETIMER_H
#define TILETIMER_H

#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/stattext.h>
#include <wx/datetime.h>

class TileTimer : public wxTimer {
    wxStaticText* timerLabel;
    wxDateTime startTime;
    wxTimeSpan pausedTime;
    wxTimeSpan timeElapsed;

public:
    TileTimer(wxStaticText* label) : timerLabel(label), startTime(wxDateTime::Now()) {}

    void Notify() override {
        timeElapsed = wxDateTime::Now() - startTime + pausedTime;
        // Update the label only if more than 1 minute has elapsed
        timerLabel->SetLabel(wxString::Format("%dm", timeElapsed.GetMinutes()));
    }

    void StartTimer() {
        timerLabel->SetForegroundColour(*wxBLUE); // Set the text color to blue
        if (!this->IsRunning()) {
            if (pausedTime == wxTimeSpan(0)) {
                // Timer is starting for the first time
                timerLabel->SetLabel(wxString::Format("%dm", timeElapsed.GetMinutes()));
                startTime = wxDateTime::Now();
            } else {
                // Resuming the timer
                startTime = wxDateTime::Now() - pausedTime;
                pausedTime = wxTimeSpan(0);
            }
            this->Start(1000); // Update every second
        } else {
            // Timer is already running, so pause it
            PauseTimer();
        }
    }

    void PauseTimer() {
        if (this->IsRunning()) {
            this->Stop();
            pausedTime += wxDateTime::Now() - startTime;
            timerLabel->SetForegroundColour(*wxLIGHT_GREY); // Set the text color to grey
        }
    }
};

#endif // TILETIMER_H
