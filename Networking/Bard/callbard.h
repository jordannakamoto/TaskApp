#include <wx/utils.h>
#include <wx/arrstr.h>

void CallBardAPI(const wxString& inputString) {
    wxString command = "python bard_api_wrapper.py \"" + inputString + "\"";
    wxArrayString output;
    wxArrayString errors;
    long exitCode = wxExecute(command, output, errors, wxEXEC_SYNC);

    if (exitCode == 0) {
        // Successfully executed, process the output
        wxString combinedOutput;
        for (const auto& line : output) {
            combinedOutput += line + "\n";
        }
        wxMessageBox(combinedOutput, "Response from Bard API");
    } else {
        // Handle errors
        wxMessageBox("Error calling Bard API", "Error", wxICON_ERROR);
    }
}
