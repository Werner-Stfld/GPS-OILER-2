import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('states')
//   {
//     "oiling": false,
//     "extraOiling": false,
//     "emergency": false,
//     "raining": false,
//     "wifi": true,
//     "washing": false
// }

    function apply () {
      console.log("states: " + JSON.stringify(settings.value))
    }
    return {
      settings,
      apply
    }
  },

  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Overview</legend>
        <labelled-input summary="Distance" desc="Distance between oil pulses in meter."/>\
        <input id="Distance" type="number" min="500" max="9999" v-model="settings.pumpDistance" name="pumpDistance"/>\
        m\
        <labelled-input summary="Washing" desc="Pumps permanently: !! Use only for service activities. !!"/>\
        <input id="Spülen" type="checkbox" v-model="settings.washing" />\
        &nbsp;\
        <labelled-input summary="Pump active" desc="Pump is currently active. (readonly)"/>\
        <input id="Pump active" type="checkbox" readonly="true" disabled="true" v-model="settings.oiling" id ="oiling" name="oiling" />\
        &nbsp;\
        <labelled-input summary="Emergency mode" desc="If GPS fails for a while and battery voltage indicates a running engine, oiling will be continued."/>\
        <input id="Emergency mode" type="checkbox" readonly="true" disabled="true" v-model="settings.emergency" name="emergency"/>\
        &nbsp;\
        <labelled-input summary="Rain" desc="Rain is detected."/>\
        <input id="Rain" type="checkbox" readonly="true" disabled="true" v-model="settings.raining" />\
        &nbsp;\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}