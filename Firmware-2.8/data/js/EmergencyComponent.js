import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('emergency')

    function apply () {
      console.log("apply emergency settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
// {
//     "timeout": 2,
//     "speed": 60
// }
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Notbetrieb</legend>\
        <labelled-input summary="Notbetrieb Timeout" desc="Zeit in Sekunden die der GPS Empfang aussetzen musss, bevor der Notbetrieb aktiviert wird. Der Wert muss mindestens 10 betragen."/>\
        <input id="Notbetrieb Timeout" type="number" min="10" v-model="settings.timeout" name="timeout"/>\
        <labelled-input summary="Geschwindigkeit" desc="Geschwindigkeit in km/h die beim Notbetrieb angenommen wird. Der Wert muss wenigstens 30 betragen."/>\
        <input id="Geschwindigkeit" type="number" min="30" v-model="settings.speed" name="speed"/>\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}