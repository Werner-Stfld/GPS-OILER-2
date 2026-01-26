import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('rain')

    function apply () {
      console.log("apply rain settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
//     "onThreshold": 150,
//     "offThreshold": 50,
//     "distanceMultiplier": 1.5,
//     "afterRainOilingPulses": 10
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Rain</legend>\
        <labelled-input summary="On threshold" desc="Theshhold to detect rain."/>\
        <input id="On threshold" type="number" min="10" max="9999" v-model="settings.onThreshold" name="onThreshold"/>\
        &nbsp;\
        <labelled-input summary="Off threshold" desc="Theshhold to detect no rain."/>\
        <input id="Off threshold" type="number" min="10" max="9999" v-model="settings.offThreshold" name="offThreshold"/>\
        &nbsp;\
        <labelled-input summary="Raining distance factor" desc="Distance multiplier while raining. Greater 1: Increase oiling intensity. Lower 1: decreases oiling intensity during rain. Min value is: 0.1"/>\
        <input id="Raining distance factor" type="number" min="0.1" max="9.9" v-model="settings.distanceMultiplier" name="distanceMultiplier" />\
        &nbsp;\
        <labelled-input summary="Pulses after rain" desc="Number of pulses after rain to reoil the chain."/>\
        <input id="Pulses after rain" type="number" min="0"  max="99" inputmode='dec' v-model="settings.afterRainOilingPulses" name="afterRainOilingPulses"/>\
        &nbsp;\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}