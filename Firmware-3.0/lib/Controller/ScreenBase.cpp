#include "ScreenBase.h"

void doNothing(){
    Serial.println("unexpected doNothing() invoked");
};

NextScreenAction ScreenBase::ScreenAction(ButtonState buttonState) {
    if (buttonState == ButtonState::LongFallingEdge) {
        execute();
        return NextScreenAction::defaultScreen;
    }
    if (buttonState == ButtonState::ShortFallingEdge) 
        return NextScreenAction::nextScreen;
    return NextScreenAction::keepScreen;
}
