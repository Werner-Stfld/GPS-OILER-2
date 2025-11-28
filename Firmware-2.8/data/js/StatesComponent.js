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
        <legend>Übersicht</legend>
        <labelled-input summary="Distanz" desc="Kilometer seit Tank Reset."/>\
        <input id="Distanz" type="number" readonly="true" disabled="true" v-model="settings.distance" name="distance"/>\
        <labelled-input summary="Distanz Ölpuls" desc="Abstand zwischen Ölpulsen in Meter."/>\
        <input id="Distanz Ölpuls" type="number" min="500" v-model="settings.pumpDistance" name="pumpDistance"/>\
        <labelled-input summary="Ölen+" desc="Extra Ölen: + 50%"/>\
        <input id="Ölen+" type="checkbox" v-model="settings.extraOiling" />
        <labelled-input summary="Spülen" desc="Spülfunktion: Fördert permanent Öl. !! Nur zu Servicezwecken aktivieren !!"/>\
        <input id="Spülen" type="checkbox" v-model="settings.washing" />\
        <labelled-input summary="Pumpe aktiv" desc="Die Kette wird gerade geölt"/>\
        <input id="Pumpe aktiv" type="checkbox" readonly="true" disabled="true" v-model="settings.oiling" id ="oiling" name="oiling" />\
        <labelled-input summary="Notbetrieb" desc="Aus der GPS-Funktion kann Geschwindigkeit nicht ermittelt werden."/>\
        <input id="Notbetrieb" type="checkbox" readonly="true" disabled="true" v-model="settings.emergency" name="emergency"/>\
        <labelled-input summary="Regen" desc="Der Regensensor erkennt Regen."/>\
        <input id="Regen" type="checkbox" readonly="true" disabled="true" v-model="settings.raining" />\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}