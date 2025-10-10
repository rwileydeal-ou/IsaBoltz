#include <macros/Macro.h>

using namespace std;

Macro::Macro(bool interactiveMode){
    interactiveMode_ = interactiveMode;
}

Macro::~Macro()
{
}

vector<string> Macro::enabledParticleKeys(vector<string> supportedParticleKeys, CommandWithPayload cmd, string cmdFriendlyName){
    vector<string> enabledParticleKeys;
    if (interactiveMode_){
        cout << "[IsaBoltz] Select all particles you wish to enable for " << cmdFriendlyName << " calculation:" << endl;
        vector<string> fullKeySelection = supportedParticleKeys;
        fullKeySelection.push_back("*");
        auto particleKeys = cmdInput(fullKeySelection);
        // if using wildcard, just return all supported keys
        if ( particleKeys[0] == "*" ){
            return supportedParticleKeys;
        }
        // not using wildcard, so choose specific keys
        for (auto& particleKey : particleKeys){
            enabledParticleKeys.push_back(particleKey);
        }
    } else{
        // if using wildcard, just return all supported keys
        if ( cmd.Payload == "*" ){
            return supportedParticleKeys;
        }
        // not using wildcard, so set specific keys
        auto payload = FileIO::Split( cmd.Payload, "&" ); 
        enabledParticleKeys = payload;
    }
    return enabledParticleKeys;
}

vector<string> Macro::cmdInput(const vector<string>& supportedOptions){
    // boot up auto-suggestions engine
    AutoSuggestions as; 
    struct AutoSuggestions::TrieNode* root = as.getNode();
    // we'll insert the supported commands for auto-suggestions
    for (auto& supportedOption : supportedOptions){
        as.insert(root, supportedOption);
    }

    // loop until we have a valid command
    while(true){
        cout << "[IsaBoltz] > ";
        string userInput;
        getline(cin, userInput);
        auto splitInput = FileIO::Split(userInput, " ");
        if (splitInput.size() > 1 && splitInput[0] == ""){
            splitInput.erase(splitInput.begin());
        }

        vector<string> commands;

        for (auto& input : splitInput){
            string trimmedInput = boost::algorithm::to_lower_copy(input);

            if (std::find(supportedOptions.begin(), supportedOptions.end(), trimmedInput) != supportedOptions.end()){
                commands.push_back( trimmedInput );
            } else{
                int comp = as.printAutoSuggestions(root, userInput);
                if (comp == 0){
                    cout << "[IsaBoltz] I'm not sure what you mean by: '" << userInput << "'" << endl;
                    cout << "[IsaBoltz] Supported options are:" << endl;
                    int tryAgain = as.printAutoSuggestions(root, "");
                    cout << "" << endl;
                    commands.clear();
                    break;
                }
            }
        }
        if (commands.size() > 0){
            return commands;
        }
    }
}

