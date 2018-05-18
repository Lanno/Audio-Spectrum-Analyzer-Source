$(function() {
    $.plot("#time_domain", [0,0]);

    $.plot("#freq_domain", [0,0]);
    
    // Message log polling updates
    
    setInterval(function() {
        var message = Queue_Wrapper.message();
        
        if(message !== "empty") {
            var elem = $("<li></li>")
                .append(message);
        
            $("#messages").append(elem);
            
        }				
    }, 1000);
    
    // Plot updates
    
    setInterval(function() {
        var fs = 4800.;
        
        var data = Queue_Wrapper.data(); 
        
        if(data !== "empty") {
            var data = JSON.parse(data);
            
            var left_data = data[0];
            
            var right_data = data[1];
            
            var left_data_pairs  = [];
            
            var right_data_pairs = [];
            
            for(var idx = 0; idx < left_data.length; idx++) {
                left_data_pairs.push([idx / fs, left_data[idx]]);
                
                right_data_pairs.push([idx / fs, right_data[idx]]);
                
            }
            
            $.plot("#time_domain", [left_data_pairs, right_data_pairs]);
            
            var left_fft = data[2];
            
            var right_fft = data[3];
            
            var left_fft_pairs  = [];
            
            var right_fft_pairs = [];
            
            for(var idx = 0; idx < left_fft.length; idx++) {
                left_fft_pairs.push([idx * fs / (2 * left_fft.length), left_fft[idx]]);
                
                right_fft_pairs.push([idx * fs / (2 * right_fft.length), right_fft[idx]]);
                
            }

            $.plot("#freq_domain", [left_fft_pairs, right_fft_pairs]);
        }
    }, 1000);
});