/**
 * Sponsor Modal
 */

let sponsorQR = null;
let sponsorConfig = null;

async function loadSponsorConfig() {
    try {
        const resp = await fetch('/api/sponsor/config');
        if (!resp.ok) return;
        sponsorConfig = await resp.json();
    } catch (e) {
        // Silently ignore — static config fallback is fine
    }
}

function openSponsorModal() {
    const overlay = document.getElementById('sponsorModalOverlay');
    if (!overlay) return;

    // Render QR code
    const qrContainer = document.getElementById('sponsorQR');
    qrContainer.innerHTML = '';

    if (sponsorConfig && sponsorConfig.qr_url) {
        sponsorQR = new QRCode(qrContainer, {
            text: sponsorConfig.qr_url,
            width: 180,
            height: 180,
            colorDark: '#16213e',
            colorLight: '#ffffff',
            correctLevel: QRCode.CorrectLevel.M,
        });
    } else {
        // Static fallback — WeChat Pay QR
        const img = document.createElement('img');
        img.src = '/api/sponsor/assets/wechatpay.png';
        img.alt = '微信支付';
        img.style.width = '180px';
        img.style.height = '180px';
        img.style.borderRadius = '8px';
        img.onerror = function () {
            qrContainer.innerHTML = '<div style="width:180px;height:180px;display:flex;align-items:center;justify-content:center;color:#666;font-size:13px;border:1px dashed #0f3460;border-radius:8px;">QR Code</div>';
        };
        qrContainer.appendChild(img);
    }

    overlay.classList.add('show');
}

function closeSponsorModal() {
    const overlay = document.getElementById('sponsorModalOverlay');
    if (overlay) overlay.classList.remove('show');
}

async function copySponsorLink() {
    const btn = document.getElementById('sponsorCopyBtn');
    const link = sponsorConfig
        ? (sponsorConfig.qr_url || sponsorConfig.repo_url || 'https://github.com/lilyco-42/clings')
        : 'https://github.com/lilyco-42/clings';

    try {
        await navigator.clipboard.writeText(link);
        if (btn) {
            const orig = btn.textContent;
            btn.textContent = '已复制';
            btn.style.borderColor = '#4ade80';
            btn.style.color = '#4ade80';
            setTimeout(() => {
                btn.textContent = orig;
                btn.style.borderColor = '';
                btn.style.color = '';
            }, 2000);
        }
    } catch (e) {
        // Fallback: open in browser
        window.open(link, '_blank');
    }
}

function saveQRCode() {
    const qrContainer = document.getElementById('sponsorQR');
    if (!qrContainer) return;

    const canvas = qrContainer.querySelector('canvas');
    const img = qrContainer.querySelector('img');

    if (canvas) {
        const link = document.createElement('a');
        link.download = 'clings-sponsor-qr.png';
        link.href = canvas.toDataURL('image/png');
        link.click();
    } else if (img && img.src) {
        const link = document.createElement('a');
        link.download = 'clings-sponsor-qr.png';
        link.href = img.src;
        link.click();
    }
}

function openTutorial(url) {
    window.open(url, '_blank');
}

// Keyboard shortcut: Escape to close
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
        closeSponsorModal();
    }
});

// Load config on init
document.addEventListener('DOMContentLoaded', loadSponsorConfig);
