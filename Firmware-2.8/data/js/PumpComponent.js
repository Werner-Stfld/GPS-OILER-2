import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('pump')

    function apply () {
      console.log("apply pump settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
// {
//     "pulsesPerMl": 50,
//     "pulseOn": 10,
//     "pulseOff":5
// }
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Pumpe</legend>\
        <labelled-input summary="Pulse pro ml" desc="Anzahl der Pumppulse um 1 ml zu verbrauchen. Der Wert wird verwendet um den Ölverbrauch zu berechnen."/>\
        <input id="Pulse pro ml" type="number" min="1" v-model="settings.pulsesPerMl" name="pulsesPerMl"/>\
        &nbsp;\
        <labelled-input summary="Puls-Ein" desc="Zeitdauer eines Pulses."/>\
        <input id="Puls-Ein" type="number" min="1" v-model="settings.pulseOn" name="pulseOn"/>\
        M-Sek\
        <labelled-input summary="Puls-Aus" desc="Mindest-Zeitpause nach einem Puls in (ms)"/>\
        <input id="Puls-Aus" type="number" min="1.0" v-model="settings.pulseOff" name="pulseOff" />\
        M-Sek\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}