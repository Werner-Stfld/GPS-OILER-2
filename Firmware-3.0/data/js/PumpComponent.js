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
        <legend>Pump</legend>\
        <labelled-input summary="Pulses per ml" desc="Number of pulses consuming 1 ml of oil. The value is used to evaluate the tank contents."/>\
        <input id="Pulses per ml" type="number" min="10" max="500" v-model="settings.pulsesPerMl" name="pulsesPerMl"/>\
        &nbsp;\
        <labelled-input summary="Puls duration" desc="Duration of a pump puls in milliseconds."/>\
        <input id="Puls duration" type="number" min="5" max="999" v-model="settings.pulseOn" name="pulseOn"/>\
        msec\
        <labelled-input summary="Puls pause" desc="Time between pump pulses in milliseconds."/>\
        <input id="Puls pause" type="number" min="50" max="999" v-model="settings.pulseOff" name="pulseOff" />\
        msec\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}