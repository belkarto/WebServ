document.addEventListener('DOMContentLoaded', function () {
    const fileInput = document.getElementById('file');

    fileInput.addEventListener('change', function () {
        const files = fileInput.files;

        if (files.length > 0) {
            const formData = new FormData();

            for (let i = 0; i < files.length; i++) {
                formData.append('files[]', files[i]);
            }

            // Make a POST request using Fetch API
            fetch('http://127.0.0.1:2001/upload', {
                method: 'POST',
                body: formData,
                headers: {
                    // You can set additional headers here if needed
                }
            })
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.text(); // You can use response.json() if the server sends JSON
            })
            .then(data => {
                // Handle the response from the server
                console.log('Server response:', data);
            })
            .catch(error => {
                // Handle errors during the fetch
                console.error('Error during fetch:', error);
            });
        }
    });
});
