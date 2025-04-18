#include "PayloadActionFactory.hpp"

#include "DelayAction.hpp"
#include "SendKeystrokesAction.hpp"

#include <algorithm>

std::vector<IPayloadAction::Ptr> PayloadActionFactory::make(const std::string& payload)
{
    const Json parsed_payload(payload);
    static constexpr const char* ACTIONS_FIELD = "actions";
    const Json actions = parsed_payload[ACTIONS_FIELD];
    return std::accumulate(actions.begin(), actions.end(), std::vector<IPayloadAction::Ptr>{}, [](std::vector<IPayloadAction::Ptr> actions, const Json& payload)
{
    actions.push_back(make_action(payload));
    return std::move(actions);
});
}

IPayloadAction::Ptr PayloadActionFactory::make_action(const Json &payload)
{
    static constexpr const char* TYPE_FIELD = "type";
    static constexpr const char* VALUE_FIELD = "value";

    const std::string type = payload[TYPE_FIELD];
    
    if (type == "delay")
    {
        const uint32_t delay_millis = payload[VALUE_FIELD];
        return std::make_unique<DelayAction>(delay_millis);
    }

    if (type == "keystrokes")
    {
        const Json value = payload[VALUE_FIELD];
        
        std::string keystroke_type = payload[TYPE_FIELD];
        std::string keystrokes = payload[VALUE_FIELD];

        std::vector<KeyStroke> parsed_keystrokes;
        if (keystroke_type == "script")
        {
            static constexpr const char *RUN_SCRIPT_PREFIX = "powershell -WindowStyle hidden -Command \"";
            static constexpr const char *RUN_SCRIPT_SUFFIX = "\"\n";
            
            keystrokes = RUN_SCRIPT_PREFIX + keystrokes + RUN_SCRIPT_SUFFIX;
            keystroke_type = "regular";
        }
        else if (keystroke_type == "encoded-script")
        {
            static constexpr const char *RUN_ENCODED_SCRIPT_PREFIX = "powershell -WindowStyle hidden -encodedCommand \"";
            static constexpr const char *RUN_ENCODED_SCRIPT_SUFFIX = "\"\n";

            keystrokes = RUN_ENCODED_SCRIPT_PREFIX + keystrokes + RUN_ENCODED_SCRIPT_SUFFIX;
            keystroke_type = "regular";
        }
        if (keystroke_type == "regular")
        {
            parsed_keystrokes = KeyStroke::from_string(keystrokes);
        }
        else if (keystroke_type == "binding")
        {
            parsed_keystrokes = KeyStroke::from_special_binding(keystrokes);
        }
        else {
            throw std::invalid_argument("invalid keystroke type");
        }

        return std::make_unique<SendKeyStrokesAction>(std::move(parsed_keystrokes));
    }

    throw std::invalid_argument("invalid action type");
}
