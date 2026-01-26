import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('display')

    function apply () {
      console.log("apply display settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
// {
//     "timezone": 1
// }
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Display</legend>\
        <labelled-input summary="Time zone" desc="Time zone used to adjust the displayed time from UTC. Eg. 1: MEZ, 2: MEZ summer."/>\
        <input id="Time zone" type="number" min="-13" max="13" v-model="settings.timeZone" name="timeZone"/>\
        hour\
        <labelled-input summary="Brightness" desc="Brightness of the display in %. Range: 1..100"/>\
        <input id="Brightness" type="number" min="1" max="100" v-model="settings.brightness" name="brightness"/>\
        %\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}