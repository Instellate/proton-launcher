function openPage(page, properties) {
    if (applicationWindow().pageStack.depth > 1) {
        applicationWindow().pageStack.pop();
    }
    applicationWindow().pageStack.push(page, properties);
}

function formatDuration(seconds) {
    const minutes = Math.floor(seconds / 60);

    if (minutes > 180) {
        const hours = Math.floor(minutes / 60);
        return i18np("1 hour", "%1 hours", hours);
    } else {
        return i18np("1 minute", "%1 minutes", minutes);
    }
}
